#pragma once
#include "../../../render/model/pmd/PMDFileStruct.h"

using RigidBodyTypeBase = decltype(PMDRigidBody::rigid_body_type);

enum class MMDRigidBodyType : RigidBodyTypeBase {
    Static,
    Dynamic,
    DynamicAdjustBone,
    Unknown,
};

MMDRigidBodyType get_mmd_rigid_body_type(const RigidBodyTypeBase& rigid_body_type);
