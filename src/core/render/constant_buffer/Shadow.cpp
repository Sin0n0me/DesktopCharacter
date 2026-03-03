#include "Shadow.h"

Shadow::Shadow(const Vector4& light_position, const Vector4& light_target) {
    const MMDMatrix view(
        DirectX::XMMatrixLookAtLH(
            light_position,
            light_target,
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        )
    );
    const MMDMatrix projection(
        DirectX::XMMatrixOrthographicLH(
            100.0f,
            100.0f,
            0.1f,
            100.0f
        )
    );

    this->set_light_view_projection(
        view,
        projection
    );
}

void Shadow::set_light_view_projection(
    const MMDMatrix& view,
    const MMDMatrix& projection
) {
    this->light_view_proj.set(
        MMDMatrix::make_model_view_projection_matrix(
            MMDMatrix::make_identity_matrix(),
            view,
            projection
        )
    );
}