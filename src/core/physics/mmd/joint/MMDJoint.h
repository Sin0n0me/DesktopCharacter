#pragma once
#include "../../../render/model/pmd/PMDFileStruct.h"
#include <memory>

class btTypedConstraint;
class MMDRigidBody;

class MMDJoint {
private:
    std::unique_ptr<btTypedConstraint>	constraint;

public:

    explicit MMDJoint(
        const PMDPhysicsJoint& joint,
        const MMDRigidBody* rigid_body_a,
        const MMDRigidBody* rigid_body_b
    );

    btTypedConstraint* get_constraint() const;
};
