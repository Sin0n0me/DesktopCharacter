#pragma once
#include "../interface_system.h"

namespace enishi::core {
    class AssetManager : public ISystem {
      private:
      public:
        void update(const types::DeltaTime& delta_time) override;
    };
} // namespace enishi::core
