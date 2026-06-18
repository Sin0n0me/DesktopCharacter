#pragma once
#include "../interface_system.h"
#include "animation_player.h"
#include <component/animation_component.h>
#include <component/ik_component.h>
#include <component/model_component.h>
#include <component/transform_component.h>
#include <ecs/registory.h>
#include <engine_types/assets/model/bone.h>
#include <memory>

namespace enishi::core {
    class AnimationSystem : public ISystem {
      private:
        std::shared_ptr<ecs::Registory> registory;

      public:
        explicit AnimationSystem(const std::shared_ptr<ecs::Registory> registory);

        void update(const types::DeltaTime& delta_time) override;

      private:
        void animation(component::AnimationComponent& animation,
            const component::ModelComponent& model,
            const component::IKComponent& ik);
    };
} // namespace enishi::core