#include "VMDAnimationBezier.h"

VMDAnimationBezier::VMDAnimationBezier(
    const std::array<std::uint8_t, 64>& interpolation
) :
    translate_x_bezier(VMDBezier::make_animation_bezier(interpolation, 0)),
    translate_y_bezier(VMDBezier::make_animation_bezier(interpolation, 1)),
    translate_z_bezier(VMDBezier::make_animation_bezier(interpolation, 2)),
    rotate_bezier(VMDBezier::make_animation_bezier(interpolation, 3)) {
}