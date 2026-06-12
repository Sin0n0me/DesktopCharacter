#pragma once
#include "../system.h"
#include <ecs/registory.h>
#include <engine_types/assets/model/bone.h>
#include <memory>

namespace enishi::core {
    class AnimationSystem : public ISystem {
      private:
        std::shared_ptr<ecs::Registory> registory;

      private:
        void update_local(const types::BoneIndex index);

        void update_global(const types::BoneIndex index);

        void update_children_global(const types::BoneIndex index);

      public:
        explicit AnimationSystem(const std::shared_ptr<ecs::Registory> registory);

        void update(const types::DeltaTime& delta_time) override;
    };
} // namespace enishi::core