#pragma once
#include "../../math/HLSLMatrix.h"
#include "../../math/WrappedMatrix.h"

// カメラ用行列
struct alignas(16) Camera {
private:
    HLSLMatrix world;
    HLSLMatrix view;
    HLSLMatrix projection;
    HLSLMatrix mvp;

public:
    explicit Camera(
        const Vector4& eye,
        const Vector4& target,
        const Vector4& up
    );

    void set_world(const MMDMatrix& world) noexcept;
    void set_view(const MMDMatrix& view) noexcept;
    void set_projection(const MMDMatrix& projection) noexcept;
    void set_mvp(const MMDMatrix& world, const MMDMatrix& view, const MMDMatrix& projection) noexcept;

    MMDMatrix get_world(void) const noexcept;
    MMDMatrix get_view(void) const noexcept;
    MMDMatrix get_projection(void) const noexcept;
};
