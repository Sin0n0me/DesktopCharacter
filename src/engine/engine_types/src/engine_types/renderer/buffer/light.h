#pragma once
#include <glm/glm.hpp>

namespace enishi::types {
    struct alignas(16) Light {
        glm::vec4 light_color;
        glm::vec4 light_direction;
    };
} // namespace enishi::types