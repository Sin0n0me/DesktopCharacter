#pragma once
#include "../math/WrappedMatrix.h"
#include <btBulletCollisionCommon.h>

btTransform matrix_to_transform(const MMDMatrix& transform) noexcept;
MMDMatrix transform_to_matrix(const btTransform& transform) noexcept;
