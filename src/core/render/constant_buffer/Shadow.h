#pragma once
#include "../../math/HLSLMatrix.h"

struct alignas(16) Shadow {
private:
    HLSLMatrix light_view_proj;

public:

    explicit Shadow(
        const Vector4& light_position,
        const Vector4& light_target
    );

    void set_light_view_projection(
        const MMDMatrix& view,
        const MMDMatrix& projection
    );
};
