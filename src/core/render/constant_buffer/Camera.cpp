#include "../../../Application.h"
#include "Camera.h"

Camera::Camera(
    const DirectX::XMVECTOR& eye,
    const DirectX::XMVECTOR& target,
    const DirectX::XMVECTOR& up
) {
    this->world = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
    this->view = DirectX::XMMatrixTranspose(
        DirectX::XMMatrixLookAtLH(eye, target, up)
    );
    this->projection = DirectX::XMMatrixTranspose(
        DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(45.0f),
            float(WIDTH) / float(HEIGHT),
            0.1f,
            200.0f
        )
    );
}