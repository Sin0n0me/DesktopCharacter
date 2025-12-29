#pragma once
#include <DirectXMath.h>

struct Camera;

struct Ray {
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 direction;

	static Ray make_ray_from_screen(const float x, const float y, const Camera& camera);
};
