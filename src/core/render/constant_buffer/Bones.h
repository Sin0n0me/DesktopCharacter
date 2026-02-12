#pragma once
#include "../../math/WrappedMatrix.h"

struct alignas(16) Bones {
    static constexpr int MAX_MATRIX_SIZE = 512;
    MMDMatrix bone_matrices[MAX_MATRIX_SIZE];
};
