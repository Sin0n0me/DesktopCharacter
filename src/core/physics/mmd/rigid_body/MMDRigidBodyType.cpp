#include "MMDRigidBodyType.h"

MMDRigidBodyType get_mmd_rigid_body_type(const RigidBodyTypeBase& rigid_body_type) {
    switch(rigid_body_type) {
    case 0:
        return MMDRigidBodyType::Static;
    case 1:
        return MMDRigidBodyType::Dynamic;
    case 2:
        return MMDRigidBodyType::DynamicAdjustBone;
    default:
        return MMDRigidBodyType::Unknown;
    }
}