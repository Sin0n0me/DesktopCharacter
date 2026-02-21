#pragma once
#include <array>
#include <cstdint>

class VMDBezier {
public:
    struct Interpolation {
        float x1;
        float y1;
        float x2;
        float y2;
    };

public:
    const Interpolation interpolation;

private:
    explicit VMDBezier(const Interpolation& interpolation);

public:

    static VMDBezier make_animation_bezier(
        const std::array<std::uint8_t, 64>& interpolation,
        const size_t offset
    );
    static VMDBezier make_camera_bezier(
        const std::array<std::uint8_t, 64>& interpolation,
        const size_t offset
    );

    float eval_x(const float t) const;
    float eval_y(const float t) const;
    float find_bezier_x(const float t) const;
};
