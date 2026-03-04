#include "Algorithm.h"
#include <algorithm>
#include <cmath>

constexpr float EPSILON = 1e-6f;

// 計算方法はChatGPTから
void jacobi_eigen_decomposition(DirectX::XMMATRIX& matrix, DirectX::XMMATRIX& eigenvectors) noexcept {
    eigenvectors = DirectX::XMMatrixIdentity();
    for(int i = 0; i < 24; ++i) {
        int p = 0;
        int q = 1;
        const float max_value = [&matrix, &p, &q]() {
            float max_value = fabsf(matrix.r[0].m128_f32[1]);
            for(int j = 0; j < 3; ++j) {
                for(int k = j + 1; k < 3; ++k) {
                    const float v = fabsf(matrix.r[j].m128_f32[k]);
                    if(v > max_value) {
                        max_value = v;
                        p = j;
                        q = k;
                    }
                }
            }
            return max_value;
            }();

        if(max_value < 1e-6f) {
            break;
        }

        const float app = matrix.r[p].m128_f32[p];
        const float aqq = matrix.r[q].m128_f32[q];
        const float apq = matrix.r[p].m128_f32[q];
        const float phi = 0.5f * atanf(2.0f * apq / (aqq - app));
        const float c = cosf(phi);
        const float s = sinf(phi);

        DirectX::XMMATRIX r = DirectX::XMMatrixIdentity();
        r.r[p].m128_f32[p] = c;
        r.r[q].m128_f32[q] = c;
        r.r[p].m128_f32[q] = s;
        r.r[q].m128_f32[p] = -s;

        matrix = DirectX::XMMatrixTranspose(r) * matrix * r;
        eigenvectors = eigenvectors * r;
    }
}

float y_bezier(const float x, const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b, const uint8_t n) {
    if(a.x == a.y && b.x == b.y) {
        return x;
    }

    float t = std::clamp(x, 0.0f, 1.0f);
    const float k0 = 1.0f + 3.0f * a.x - 3.0f * b.x;
    const float k1 = 3.0f * b.x - 6.0f * a.x;
    const float k2 = 3.0f * a.x;
    for(int i = 0; i < n; ++i) {
        const float s0 = k0 * t * t * t;
        const float s1 = k1 * t * t;
        const float s2 = k2 * t - x;
        const float df = s0 + s1 + s2;
        if(std::abs(df) < EPSILON) {
            break;
        }
        // 念のため
        if(std::abs(t) > 1.0f) {
            t = std::clamp(t, 0.0f, 1.0f);
            break;
        }

        t -= df / 2.0f;
    }

    return cubic_bezier(t, 1.0f, b.y, a.y, 0.0f);
}

float cubic_bezier(const float t, const float p0, const float p1, const float p2, const float p3) noexcept {
    const float u = 1.0f - t;
    const float s0 = t * t * t * p0;
    const float s1 = 3.0f * t * t * u * p1;
    const float s2 = 3.0f * t * u * u * p2;
    const float s3 = u * u * u * p3;
    return s0 + s1 + s2 + s3;
}

DirectX::XMVECTOR slerp_quaternion(const DirectX::XMVECTOR& q0, const DirectX::XMVECTOR& q1, const float t) noexcept {
    DirectX::XMVECTOR q1_adj = q1;

    // 内積を取る
    float dot = DirectX::XMVectorGetX(DirectX::XMQuaternionDot(q0, q1));

    // 最短経路補正
    if(dot < 0.0f) {
        q1_adj = DirectX::XMVectorNegate(q1_adj);
        dot = -dot;
    }

    // 角度が小さい場合は LERP + 正規化
    if(dot > 1.0f - EPSILON) {
        DirectX::XMVECTOR q = DirectX::XMVectorLerp(q0, q1_adj, t);
        return DirectX::XMQuaternionNormalize(q);
    }

    // SLERP
    return DirectX::XMQuaternionSlerp(q0, q1_adj, t);
}