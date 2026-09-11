#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace enishi::component {
    struct PhysicsComponent {
        std::vector<glm::mat4> local;
        std::vector<glm::mat4> global;
    };
} // namespace enishi::component
