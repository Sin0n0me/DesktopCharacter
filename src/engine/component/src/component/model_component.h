#pragma once
#include <engine_types/assets/model/addons/bone.h>
#include <vector>

namespace enishi::component {
    struct ModelComponent {
        std::vector<types::AnimationBone> animation;
        std::vector<types::PhysicsBone> physics;
        std::vector<types::BindBone> bind;
        std::vector<types::SkinningBone> skinning;
        std::vector<types::BoneNode> node;
    };
} // namespace enishi::component
