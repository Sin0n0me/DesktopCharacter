#pragma once
#include "WrappedMatrix.h"

class HLSLMatrix {
private:
    Matrix4x4 matrix;

public:

    explicit HLSLMatrix(void) : matrix(Matrix::make_identity_matrix().get()) {}
    //explicit HLSLMatrix(const Matrix4x4& matrix) : matrix(matrix) {}

    template <bool InputHand, bool IsRowMajor>
    void set(const WrappedMatrix<InputHand, IsRowMajor>& matrix) noexcept {
#ifdef USE_GLM
        static_assert(false);
#else
        // HLSL上では列優先として扱っているが, DirectXMath自体が行優先のメモリ配置となっているので
        // このラッパークラスでメモリ上の配置を変える
        if constexpr(IsRowMajor) {
            this->matrix = matrix.get();
        } else {
            this->matrix = matrix.transpose().get();
        }
#endif // USE_GLM
    }

    MMDMatrix get(void) const noexcept {
        return MMDMatrix(this->matrix); // 転置は不要
    }
};
