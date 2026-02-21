#pragma once
#include "Bezier.h"

struct VMDAnimationBezier {
public:
    const VMDBezier translate_x_bezier;
    const VMDBezier translate_y_bezier;
    const VMDBezier translate_z_bezier;
    const VMDBezier rotate_bezier;

    explicit VMDAnimationBezier(const std::array<std::uint8_t, 64>& interpolation);
};
