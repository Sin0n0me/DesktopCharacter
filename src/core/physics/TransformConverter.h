#pragma once
#include "../math/WrappedMatrix.h"
#include <btBulletCollisionCommon.h>

btTransform matrix_to_transform(const BulletMatrix& transform);
BulletMatrix transform_to_matrix(const btTransform& transform);
