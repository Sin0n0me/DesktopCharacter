#include "Collider.h"
#include "../utility/Utility.h"

bool Collider::hit_model(const Ray& ray, const OBB& obb) {
	const DirectX::XMVECTOR ray_origin = DirectX::XMLoadFloat3(&ray.origin);
	const DirectX::XMVECTOR ray_dir = DirectX::XMLoadFloat3(&ray.direction);
	const DirectX::XMVECTOR obb_center = DirectX::XMLoadFloat3(&obb.center);
	const DirectX::XMVECTOR delta = sub_float(obb_center, ray_origin);

	float t_min = 0.0f;
	float t_max = FLT_MAX;
	for(int axis_index = 0; axis_index < 3; ++axis_index) {
		const DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(
			&obb.axis[axis_index]
		);
		const float e = DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(axis, delta)
		);
		const float f = DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(axis, ray_dir)
		);
		// xのアドレスを取ってindexによるx, y, zの順序で値を取得
		const float extent = (&obb.half_extent.x)[axis_index];

		if(fabsf(f) > 1e-6f) {
			float t1 = (e + extent) / f;
			float t2 = (e - extent) / f;

			if(t1 > t2) {
				const float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			t_min = t1 > t_min ? t1 : t_min;
			t_max = t2 < t_max ? t2 : t_max;

			if(t_min > t_max) {
				return false;
			}
		} else {
			// レイがスラブと平行
			if(-e - extent > 0.0f || -e + extent < 0.0f) {
				return false;
			}
		}
	}

	return true;
}