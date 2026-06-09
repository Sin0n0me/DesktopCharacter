#pragma once
#include <cstdint>

namespace enishi::types {
    struct Camera {
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 mvp;
    };
} // namespace enishi::types