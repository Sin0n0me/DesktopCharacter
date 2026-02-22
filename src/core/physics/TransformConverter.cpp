#include "TransformConverter.h"

btTransform matrix_to_transform(const BulletMatrix& matrix) {
    btTransform transform;
    transform.setFromOpenGLMatrix(
        matrix.to_column_major_array().data()
    );
    return transform;
}

BulletMatrix transform_to_matrix(const btTransform& transform) {
    std::array<float, 16> array{};
    transform.getOpenGLMatrix(array.data());
    return BulletMatrix(array);
}