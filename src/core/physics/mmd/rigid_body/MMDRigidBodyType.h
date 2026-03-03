#pragma once
#include "../../../render/model/pmd/PMDFileStruct.h"

using RigidBodyTypeBase = decltype(PMDRigidBody::rigid_body_type);

enum class MMDRigidBodyType : RigidBodyTypeBase {
    Kinematic = 0,          // ボーン追従
    Dynamic = 1,            //
    DynamicAdjustBone = 2,  //
};
