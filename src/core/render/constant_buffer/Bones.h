#pragma once
#include <DirectXMath.h>

constexpr int MAX_MATRIX_SIZE = 256;

struct Bones {
	DirectX::XMMATRIX bone_matrices[MAX_MATRIX_SIZE];
};
