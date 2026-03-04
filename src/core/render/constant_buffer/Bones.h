#pragma once
#include "../../math/HLSLMatrix.h"

struct alignas(16) Bones {
    static constexpr int MAX_MATRIX_SIZE = 512;
    HLSLMatrix bone_matrices[MAX_MATRIX_SIZE];
};
