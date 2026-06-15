#pragma once
#include <engine_types/handle/handle_type.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <set>
#include <unordered_set>
#include <vector>

namespace enishi::component {
    struct SkeletonComponent {
        std::vector<glm::mat4> bind_bone_matrices; // バインドボーン
    };
} // namespace enishi::component
