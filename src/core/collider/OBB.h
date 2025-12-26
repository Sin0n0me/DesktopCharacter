#pragma once
#include <DirectXMath.h>

constexpr float EXTENT_MARGIN = 1.1f;

struct OBB {
	DirectX::XMFLOAT3 center;       // ローカル空間
	DirectX::XMFLOAT3 axis[3];      // 正規化された軸（ローカル）
	DirectX::XMFLOAT3 half_extent;  // 半径
};
