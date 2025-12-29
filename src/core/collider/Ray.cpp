#include "Ray.h"
#include "../constant_buffer/Camera.h"
#include "../../Application.h"

Ray Ray::make_ray_from_screen(const float x, const float y, const Camera& camera) {
	// スクリーン座標からNDCに
	const float ndc_x = (2.0f * x / WIDTH) - 1.0f;
	const float ndc_y = 1.0f - (2.0f * y / HEIGHT);

	// クリップ空間（z = 1 は far plane）
	const DirectX::XMVECTOR ray_clip = DirectX::XMVectorSet(ndc_x, ndc_y, 1.0f, 1.0f);

	// ビュー空間へ逆変換
	const DirectX::XMMATRIX inverse_projection = DirectX::XMMatrixInverse(
		nullptr,
		DirectX::XMMatrixTranspose(camera.projection) // 転置しているので再度転置が必要
	);
	DirectX::XMVECTOR ray_view = DirectX::XMVector4Transform(
		ray_clip,
		inverse_projection
	);

	// 方向ベクトルなのでwを0に
	ray_view = DirectX::XMVectorSetW(ray_view, 0.0f);

	// ワールド空間へ逆変換
	const DirectX::XMMATRIX inverse_view = DirectX::XMMatrixInverse(
		nullptr,
		DirectX::XMMatrixTranspose(camera.view) // 転置しているので再度転置が必要
	);
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