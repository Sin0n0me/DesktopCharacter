#pragma once

#include <cstdint>
#include <unordered_map>
#include <DirectXMath.h>

struct PMDBoneData {
	int parent;
	DirectX::XMFLOAT3 position;
	DirectX::XMMATRIX local;
	DirectX::XMMATRIX global;
	DirectX::XMMATRIX inverse_bind;
};

using BoneIndex = uint16_t;
using PMDBoneMap = std::unordered_map<BoneIndex, PMDBoneData>;