#pragma once
#include "../../../../math/WrappedMatrix.h"
#include <DirectXMath.h>

struct BindBone {
    DirectX::XMFLOAT3 position;
    MMDMatrix local;
    MMDMatrix global;
    MMDMatrix inverse;
};