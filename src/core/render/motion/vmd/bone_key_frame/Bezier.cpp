#include "Bezier.h"

float VMDBezier::eval_x(const float t) const {
    const float it = 1.0f - t;
    const float x[4] = {
        0.0f,
        this->interpolation.x1,
        this->interpolation.x2,
        1.0f,
    };

    return t * t * t * x[3]
        + 3.0f * t * t * it * x[2]
        + 3.0f * t * it * it * x[1]
        + it * it * it * x[0];
}

float VMDBezier::eval_y(const float t) const {
    const float it = 1.0f - t;
    const float x[4] = {
        0.0f,
        this->interpolation.y1,
        this->interpolation.y2,
        1.0f,
    };

    return t * t * t * x[3]
        + 3.0f * t * t * it * x[2]
        + 3.0f * t * it * it * x[1]
        + it * it * it * x[0];
}

float VMDBezier::find_bezier_x(const float t) const {
    const float EPSILON = 1e-5f;
    float start = 0.0f;
    float stop = 1.0f;
    float t2 = 0.5f;
    for(
        float x = this->eval_x(t2);
        EPSILON < std::abs(t - x);
        x = this->eval_x(t2)
        ) {
        if(t < x) {
            stop = t2;
        } else {
            start = t2;
        }
        t2 = (stop + start) * 0.5f;
    }

    return t2;
}

VMDBezier::VMDBezier(const Interpolation& interpolation) :
    interpolation(interpolation) {
}

VMDBezier VMDBezier::make_animation_bezier(
    const std::array<std::uint8_t, 64>& interpolation,
    const size_t offset
) {
    const int x0 = interpolation[0 + offset];
    const int y0 = interpolation[4 + offset];
    const int x1 = interpolation[8 + offset];
    const int y1 = interpolation[12 + offset];

    return VMDBezier(
        VMDBezier::Interpolation{
            .x1 = static_cast<float>(x0) / 127.0f,
            .y1 = static_cast<float>(y0) / 127.0f,
            .x2 = static_cast<float>(x1) / 127.0f,
            .y2 = static_cast<float>(y1) / 127.0f
        }
    );
}

VMDBezier VMDBezier::make_camera_bezier(
    const std::array<std::uint8_t, 64>& interpolation,
    const size_t offset
) {
    const int x0 = interpolation[0 + offset];
    const int y0 = interpolation[1 + offset];
    const int x1 = interpolation[2 + offset];
    const int y1 = interpolation[3 + offset];

    return VMDBezier(
        VMDBezier::Interpolation{
            .x1 = static_cast<float>(x0) / 127.0f,
            .y1 = static_cast<float>(y0) / 127.0f,
            .x2 = static_cast<float>(x1) / 127.0f,
            .y2 = static_cast<float>(y1) / 127.0f
        }
    );
}