#pragma once
#include <DirectXMath.h>

struct alignas(16) Shadow {
	DirectX::XMMATRIX light_view_proj;
};
