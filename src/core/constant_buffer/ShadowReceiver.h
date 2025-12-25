#pragma once
#include <DirectXMath.h>

struct ShadowReceiver {
	DirectX::XMMATRIX view_proj;
	DirectX::XMMATRIX light_view_proj;
};
