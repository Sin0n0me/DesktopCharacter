#include "../../../Application.h"
#include "Camera.h"

Camera::Camera(
    const Vector4& eye,
    const Vector4& target,
    const Vector4& up
) {
    this->set_mvp(
        MMDMatrix::make_identity_matrix(),
        MMDMatrix(
            DirectX::XMMatrixLookAtLH(eye, target, up)
        ),
        MMDMatrix(
            DirectX::XMMatrixPerspectiveFovLH(
                DirectX::XMConvertToRadians(45.0f),
                float(WIDTH) / float(HEIGHT),
                0.1f,
                200.0f
            )
        )
    );
}

void Camera::set_world(const MMDMatrix& world) noexcept {
    this->world.set(world);
}

void Camera::set_view(const MMDMatrix& view) noexcept {
    this->view.set(view);
}

void Camera::set_projection(const MMDMatrix& projection) noexcept {
    this->projection.set(projection);
}

void Camera::set_mvp(const MMDMatrix& world, const MMDMatrix& view, const MMDMatrix& projection) noexcept {
    this->world.set(world);
    this->view.set(view);
    this->projection.set(projection);
    this->mvp.set(
        MMDMatrix::make_model_view_projection_matrix(
            world,
            view,
            projection
        )
    );
}

MMDMatrix Camera::get_world(void) const noexcept {
    return this->world.get();
}

MMDMatrix Camera::get_view(void) const noexcept {
    return this->view.get();
}

MMDMatrix Camera::get_projection(void) const noexcept {
    return this->projection.get();
}