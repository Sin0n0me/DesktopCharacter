#include "../utility/Algorithm.h"
#include "OBB.h"

OBB OBB::make_by_covariance_matrix(const std::vector<DirectX::XMFLOAT3>& positions) {
    const float positions_size = static_cast<float>(positions.size());

    // 重心を求める
    DirectX::XMVECTOR mean = DirectX::XMVectorZero();
    for(const auto& position : positions) {
        mean = DirectX::XMVectorAdd(mean, DirectX::XMLoadFloat3(&position));
    }
    mean = DirectX::XMVectorScale(mean, 1.0 / positions_size);

    // 共分散行列の作成
    DirectX::XMMATRIX matrix = DirectX::XMMatrixSet(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );
    for(const auto& position : positions) {
        const auto vec_diff = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position), mean);
        DirectX::XMFLOAT3 df;
        DirectX::XMStoreFloat3(&df, vec_diff);

        // vec_diff.x * vec_diff (x*x, x*y, x*z, x*w)
        matrix.r[0] = DirectX::XMVectorMultiplyAdd(
            DirectX::XMVectorSplatX(vec_diff),
            vec_diff,
            matrix.r[0]
        );
        // vec_diff.y * vec_diff (y*x, y*y, y*z, y*w)
        matrix.r[1] = DirectX::XMVectorMultiplyAdd(
            DirectX::XMVectorSplatY(vec_diff),
            vec_diff,
            matrix.r[1]
        );
        // vec_diff.z * vec_diff (z*x, z*y, z*z, z*w)
        matrix.r[2] = DirectX::XMVectorMultiplyAdd(
            DirectX::XMVectorSplatZ(vec_diff),
            vec_diff,
            matrix.r[2]
        );
    }
    matrix *= 1.0f / positions_size;
    matrix.r[3] = DirectX::XMVectorSet(0, 0, 0, 1.0f); // 元の行列は単位行列ではないので

    // ヤコビ法による固有ベクトル取得
    DirectX::XMMATRIX eigenvectors{};
    jacobi_eigen_decomposition(matrix, eigenvectors);

    return OBB::make(positions, mean, eigenvectors);
}

OBB OBB::make(
    const std::vector<DirectX::XMFLOAT3>& positions,
    const DirectX::XMVECTOR& mean,
    const DirectX::XMMATRIX& eigen_vectors
) {
    // OBBの作成
    OBB obb{};
    for(int i = 0; i < 3; ++i) {
        const DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(eigen_vectors.r[i]);
        DirectX::XMStoreFloat3(&obb.axis[i], axis);
    }

    // 軸空間でAABB
    const DirectX::XMVECTOR minV = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
    const DirectX::XMVECTOR maxV = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);

    // 各軸の最小値と最大値を求める
    DirectX::XMVECTOR min_vec = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
    DirectX::XMVECTOR max_vec = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);
    for(const auto& position : positions) {
        const DirectX::XMVECTOR d = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position), mean);
        const DirectX::XMVECTOR local = DirectX::XMVectorSet(
            DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[0]))),
            DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[1]))),
            DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[2]))),
            0.0f
        );
        min_vec = DirectX::XMVectorMin(min_vec, local);
        max_vec = DirectX::XMVectorMax(max_vec, local);
    }

    const DirectX::XMVECTOR half = DirectX::XMVectorScale(DirectX::XMVectorSubtract(max_vec, min_vec), 0.5f);
    const DirectX::XMVECTOR vec_offset = DirectX::XMVectorScale(DirectX::XMVectorAdd(max_vec, min_vec), 0.5f);
    DirectX::XMStoreFloat3(&obb.half_extent, half);

    // 各軸のオフセットを求める
    DirectX::XMFLOAT3 offset;
    DirectX::XMStoreFloat3(&offset, vec_offset);
    const auto axis_offset_x = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&obb.axis[0]), offset.x);
    const auto axis_offset_y = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&obb.axis[1]), offset.y);
    const auto axis_offset_z = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&obb.axis[2]), offset.z);

    DirectX::XMVECTOR offset_mean = mean;
    offset_mean = DirectX::XMVectorAdd(offset_mean, axis_offset_x);
    offset_mean = DirectX::XMVectorAdd(offset_mean, axis_offset_y);
    offset_mean = DirectX::XMVectorAdd(offset_mean, axis_offset_z);

    // 中心補正
    DirectX::XMStoreFloat3(&obb.center, offset_mean);

    return obb;
}