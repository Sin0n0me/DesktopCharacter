#pragma once
#include <glm/glm.hpp>

namespace enishi::types {
    struct alignas(16) Bones {
        static constexpr int MAX_MATRIX_SIZE = 512;
        glm::mat4 bone_matrices[MAX_MATRIX_SIZE];
    };
} // namespace enishi::types