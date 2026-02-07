#pragma once
#include <DirectXMath.h>

struct BindBone {
    DirectX::XMFLOAT3 position;
    DirectX::XMMATRIX local;
    DirectX::XMMATRIX global;
    DirectX::XMMATRIX inverse;
};