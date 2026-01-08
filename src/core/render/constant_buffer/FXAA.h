#pragma once
#include <DirectXMath.h>

struct alignas(16) FXAA {
	DirectX::XMFLOAT2 inverse_screen_size;
	float span_max;
	float reduce_min;
	float reduce_mul;
};
