#pragma once
#include "../../../../math/WrappedMatrix.h"

struct BindBone {
    Vector4 position;
    MMDMatrix local;
    MMDMatrix global;
    MMDMatrix global_inverse;
};