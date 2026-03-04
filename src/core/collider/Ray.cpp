#include "../../Application.h"
#include "../render/constant_buffer/Camera.h"
#include "Ray.h"

// TODO: ラッパークラスへ移行
Ray Ray::make_ray_from_screen(const float x, const float y, const Camera& camera) {
    // スクリーン座標からNDCに
    const float ndc_x = (2.0f * x / WIDTH) - 1.0f;
    const float ndc_y = 1.0f - (2.0f * y / HEIGHT);

    // クリップ空間（z = 1 は far plane）
    const DirectX::XMVECTOR ray_clip = DirectX::XMVectorSet(ndc_x, ndc_y, 1.0f, 1.0f);

    // ビュー空間へ逆変換
    const DirectX::XMMATRIX inverse_projection = camera.get_projection().inverse().transpose().get();
    DirectX::XMVECTOR ray_view = DirectX::XMVector4Transform(
        ray_clip,
        inverse_projection
    );

    // 方向ベクトルなのでwを0に
    ray_view = DirectX::XMVectorSetW(ray_view, 0.0f);

    // ワールド空間へ逆変換
    const DirectX::XMMATRIX inverse_view = camera.get_view().inverse().transpose().get();
    DirectX::XMVECTOR ray_direction = DirectX::XMVector3Normalize(
        DirectX::XMVector3TransformNormal(ray_view, inverse_view)
    );

    // レイの原点（カメラ位置）
    const DirectX::XMVECTOR ray_origin = inverse_view.r[3];

    Ray ray{};
    XMStoreFloat3(&ray.origin, ray_origin);
    XMStoreFloat3(&ray.direction, ray_direction);

    return ray;
}