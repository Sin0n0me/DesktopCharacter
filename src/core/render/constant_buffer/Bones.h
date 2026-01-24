#pragma once
#include <DirectXMath.h>

constexpr int MAX_MATRIX_SIZE = 512;

struct alignas(16) Bones {
    DirectX::XMMATRIX bone_matrices[MAX_MATRIX_SIZE];
};
