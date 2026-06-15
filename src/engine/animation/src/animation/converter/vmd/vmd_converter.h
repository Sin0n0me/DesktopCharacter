#pragma once
#include <assets_system/animation/vmd/vmd_data.h>
#include <engine_types/assets/animation/animation.h>

namespace enishi::animation {
    //
    class FrameConverter {
      public:
        static types::Keyframes<int> to_keyframes(const assets_system::VMDData& data);

      private:
    };
} // namespace enishi::animation
