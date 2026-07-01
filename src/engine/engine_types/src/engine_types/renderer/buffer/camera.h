#pragma once
#include <glm/glm.hpp>

namespace enishi::types {
    struct alignas(16) Camera {
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 mvp;
    };
} // namespace enishi::types