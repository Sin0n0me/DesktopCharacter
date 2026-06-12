#include "animation_system.h"

namespace enishi::core {
    void AnimationSystem::update_local(const types::BoneIndex index) {
    }

    void AnimationSystem::update_global(const types::BoneIndex index) {
    }

    void AnimationSystem::update_children_global(const types::BoneIndex index) {
    }

    AnimationSystem::AnimationSystem(const std::shared_ptr<ecs::Registory> registory)
        : registory(registory) {
        const auto& a = this->registory->get<types::BoneNode>(0);
    }

    void AnimationSystem::update(const types::DeltaTime& delta_time) {
    }
} // namespace enishi::core