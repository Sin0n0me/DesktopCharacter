#pragma once
#include "../../../render/model/pmd/PMDFileStruct.h"

using RigidBodyShapeTypeBase = decltype(PMDRigidBody::shape_type);

enum class MMDRigidBodyShape : RigidBodyShapeTypeBase {
    Sphere = 0,
    Box = 1,
    Capsule = 2,
};
