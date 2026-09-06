#pragma once
#include <engine_types/assets/model/addons/bone.h>
#include <glm/glm.hpp>

namespace enishi::platform {
    class ICollisionHandler {
      public:
        virtual ~ICollisionHandler(void) noexcept = default;

        virtual void on_collision();
    };
} // namespace enishi::platform