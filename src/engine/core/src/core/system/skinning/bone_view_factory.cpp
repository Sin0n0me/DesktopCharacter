#include "bone_view_factory.h"

namespace enishi::core {
    skinning_system::AnimationBoneView BoneViewFactory::make_animation_view(
        component::AnimationComponent& animation, const types::BoneIndex index) noexcept {
        auto& bone = animation.animation[index];

        return skinning_system::AnimationBoneView{
            bone.position,
            bone.rotation,
            bone.scale,
            animation.global[index],
        };
    }

    skinning_system::IKBoneView BoneViewFactory::make_ik_view(
        component::IKComponent& ik, const types::BoneIndex index) noexcept {
        return skinning_system::IKBoneView{};
    }

    skinning_system::PhysicsBoneView BoneViewFactory::make_physics_view(
        component::PhysicsComponent& physics, const types::BoneIndex index) noexcept {
        return skinning_system::PhysicsBoneView{
            physics.local[index],
            physics.global[index],
        };
    }

    skinning_system::BindBoneView BoneViewFactory::make_bind_view(
        const component::ModelComponent& model, const types::BoneIndex index) noexcept {
        const auto& bind_bone = model.bind_bone;
        return skinning_system::BindBoneView{
            bind_bone[index].local,
            bind_bone[index].global,
            bind_bone[index].global_inverse,
        };
    }
} // namespace enishi::core