#pragma once
#include <glm/glm.hpp>

namespace enishi::platform {
    class IModelView {
      public:
        virtual ~IModelView(void) noexcept = default;
    };
} // namespace enishi::platform