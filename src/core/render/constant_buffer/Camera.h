#pragma once
#include <DirectXMath.h>

// カメラ用行列
struct alignas(16) Camera {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;

    explicit Camera(
        const DirectX::XMVECTOR& eye,
        const DirectX::XMVECTOR& target,
        const DirectX::XMVECTOR& up
    );
};
