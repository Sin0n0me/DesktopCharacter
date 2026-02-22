#pragma once
#include "../../../render/model/pmd/PMDFileStruct.h"

using RigidBodyShapeTypeBase = decltype(PMDRigidBody::shape_type);

enum class MMDRigidBodyShape : RigidBodyShapeTypeBase {
    Sphere,
    Box,
    Capsule,
    Unknown,
};

MMDRigidBodyShape get_mmd_rigid_body_shape(const RigidBodyShapeTypeBase& rigid_body_type) {
    switch(rigid_body_type) {
    case 0:
        return MMDRigidBodyShape::Sphere;
    case 1:
        return MMDRigidBodyShape::Box;
    case 2:
        return MMDRigidBodyShape::Capsule;
    default:
        return MMDRigidBodyShape::Unknown;
    }
}
