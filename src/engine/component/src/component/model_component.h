#pragma once
#include <engine_types/assets/model/bone.h>
#include <glm/glm.hpp>

namespace enishi::component {
    struct ModelComponent {
        std::vector<types::BoneNode> bone_node; // 接続先などの情報
    };
} // namespace enishi::component
