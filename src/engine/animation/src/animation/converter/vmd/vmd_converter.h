#pragma once
#include <assets_system/animation/vmd/vmd_data.h>
#include <assets_system/model/interface_bone_resolver.h>
#include <engine_types/assets/animation/animation.h>

namespace enishi::animation {
    //
    class FrameConverter {
      public:
        static types::AnimationClipData make_clip_data(
            const assets_system::IBoneResolver* resolver, const assets_system::VMDData& data);

      private:
    };
} // namespace enishi::animation
