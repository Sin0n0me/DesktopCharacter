#pragma once
#include <component/animation_component.h>
#include <component/ik_component.h>
#include <component/model_component.h>
#include <component/physics_component.h>
#include <engine_types/assets/model/addons/bone.h>
#include <skinning_system/views/animation_bone_view.h>
#include <skinning_system/views/bind_bone_view.h>
#include <skinning_system/views/ik_bone_view.h>
#include <skinning_system/views/physics_bone_view.h>

namespace enishi::core {
    /**
     * この型の目的
     *
     * component(ECS)側のデータと、skinning_system側のView実装(platformインターフェース)を結びつける
     * ここだけがcomponentとskinning_systemの両方を知っていてよい場所とする
     *
     * IK・物理演算といった各システムは、ここが生成したViewをplatformのインターフェース越しに
     * 受け取るだけなので、component/ecsの存在を一切知らずに済む
     */
    class BoneViewFactory final {
      public:
        BoneViewFactory(void) = delete;

        [[nodiscard]] static skinning_system::AnimationBoneView make_animation_view(
            component::AnimationComponent& animation, const types::BoneIndex index) noexcept;

        [[nodiscard]] static skinning_system::PhysicsBoneView make_physics_view(
            component::PhysicsComponent& physics, const types::BoneIndex index) noexcept;

        [[nodiscard]] static skinning_system::BindBoneView make_bind_view(
            const component::ModelComponent& model, const types::BoneIndex index) noexcept;
    };
} // namespace enishi::core
