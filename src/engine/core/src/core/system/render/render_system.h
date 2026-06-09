#pragma once
#include "../system.h"
#include <ecs/registory.h>

namespace enishi::core {
    class RenderSystem : public ISystem {
      private:
        ecs::Registory regstory;

      public:
        void update(const types::DeltaTime& delta_time) override;
    };
} // namespace enishi::core
