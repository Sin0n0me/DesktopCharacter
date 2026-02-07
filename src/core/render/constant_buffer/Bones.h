#pragma once
#include <DirectXMath.h>

struct alignas(16) Bones {
    static constexpr int MAX_MATRIX_SIZE = 512;
    DirectX::XMMATRIX bone_matrices[MAX_MATRIX_SIZE];
};
