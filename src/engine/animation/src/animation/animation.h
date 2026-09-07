#pragma once
#include <component/animation_component.h>

namespace enishi::animation {
    // ロジックのみ
    class Animation {
      public:
        static void apply_animation(
            component::AnimationComponent& animation, const float elapsed_time);
    };
} // namespace enishi::animation