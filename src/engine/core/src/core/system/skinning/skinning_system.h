#pragma once
#include "model_bones.h"
#include <component/animation_component.h>
#include <component/ik_component.h>
#include <component/model_component.h>
#include <component/physics_component.h>
#include <component/skinning_component.h>
#include <core/system/interface_system.h>
#include <ecs/registory.h>
#include <memory>
#include <unordered_map>

namespace enishi::core {
    /**
     * モデルが持つ各ボーンデータを適切な順序で計算し
     * 最終的にスキニング用の行列を作成する
     */
    class SkinningSystem final : public ISystem {
      private:
        ecs::Registory* const registory;
        std::unordered_map<types::HandleId, std::unique_ptr<ModelBones>> model_bones;

      public:
        explicit SkinningSystem(ecs::Registory& registory) noexcept;

        bool should_close(void) override;
        void pre_update(void) override;
        void update(const types::DeltaTime& delta_time) override;
        void post_update(void) override;
        void render(void) const override;

      private:
        // 初めて見るエンティティであればModelBonesを構築する
        [[nodiscard]] ModelBones& get_or_build(const types::HandleId entity,
            component::AnimationComponent& animation,
            const component::ModelComponent& model,
            component::IKComponent& ik,
            component::PhysicsComponent& physics) noexcept;

        void solve_ik(ModelBones& bones, const component::IKComponent& ik) const noexcept;

        void write_skinning_matrices(const component::AnimationComponent& animation,
            const component::ModelComponent& model,
            component::SkinningComponent& skinning) const noexcept;
    };
} // namespace enishi::core
