#pragma once
#include <DirectXMath.h>

struct BoneNode {
    DirectX::XMVECTOR rotate;
    DirectX::XMMATRIX local;
    DirectX::XMMATRIX global;
};
