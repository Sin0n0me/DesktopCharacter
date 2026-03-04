#include "TransformConverter.h"

btTransform matrix_to_transform(const BulletMatrix& matrix) noexcept {
    btTransform transform;
    const std::array<btScalar, 16> m = matrix.to_column_major_array();
    transform.setFromOpenGLMatrix(
        m.data()
    );
    return transform;
}

BulletMatrix transform_to_matrix(const btTransform& transform) noexcept {
    std::array<btScalar, 16> array{};
    transform.getOpenGLMatrix(array.data());
    return BulletMatrix(array);
}