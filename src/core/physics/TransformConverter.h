#pragma once
#include "../math/WrappedMatrix.h"
#include <LinearMath/btTransform.h>

btTransform matrix_to_transform(const BulletMatrix& transform) noexcept;
BulletMatrix transform_to_matrix(const btTransform& transform) noexcept;
