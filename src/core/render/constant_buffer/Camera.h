#pragma once
#include <DirectXMath.h>

// ƒJƒƒ‰—ps—ñ
struct alignas(16) Camera {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};
