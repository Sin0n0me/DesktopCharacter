#pragma once

#include <engine_types/system/delta_time.h>

namespace enishi::core {
    class ISystem {
      public:
        virtual ~ISystem(void) noexcept = default;

        virtual void update(const types::DeltaTime& delta_time) = 0;
    };
} // namespace enishi::core