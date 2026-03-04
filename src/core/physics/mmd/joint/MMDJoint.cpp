#include "../../../log/Logger.h"
#include "../rigid_body/MMDRigidBody.h"
#include "MMDJoint.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

// PMDのJoint情報を基にconstraintを作成
MMDJoint::MMDJoint(
    const PMDPhysicsJoint& joint,
    const MMDRigidBody* rigid_body_a,
    const MMDRigidBody* rigid_body_b
) {
    btMatrix3x3 rotate_matrix;
    rotate_matrix.setEulerZYX(
        joint.rotation[0],
        joint.rotation[1],
        joint.rotation[2]
    );

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(
        btVector3(
            joint.position[0],
            joint.position[1],
            joint.position[2]
        )
    );
    transform.setBasis(rotate_matrix);

    const auto bt_rigid_body_a = rigid_body_a->get_rigid_body();
    const auto bt_rigid_body_b = rigid_body_b->get_rigid_body();
    const btTransform inverse_a = bt_rigid_body_a->getWorldTransform().inverse() * transform;
    const btTransform inverse_b = bt_rigid_body_b->getWorldTransform().inverse() * transform;
    auto constraint = std::make_unique<btGeneric6DofSpringConstraint>(
        *bt_rigid_body_a,
        *bt_rigid_body_b,
        inverse_a,
        inverse_b,
        true
    );
    constraint->setLinearLowerLimit(
        btVector3(
            joint.constrain_position_min[0],
            joint.constrain_position_min[1],
            joint.constrain_position_min[2]
        )
    );
    constraint->setLinearUpperLimit(
        btVector3(
            joint.constrain_position_max[0],
            joint.constrain_position_max[1],
            joint.constrain_position_max[2]
        )
    );

    constraint->setAngularLowerLimit(
        btVector3(
            joint.constrain_rotation_min[0],
            joint.constrain_rotation_min[1],
            joint.constrain_rotation_min[2]
        )
    );
    constraint->setAngularUpperLimit(
        btVector3(
            joint.constrain_rotation_max[0],
            joint.constrain_rotation_max[1],
            joint.constrain_rotation_max[2]
        )
    );

    if(joint.spring_position[0] != 0) {
        constraint->enableSpring(0, true);
        constraint->setStiffness(0, joint.spring_position[0]);
    }
    if(joint.spring_position[1] != 0) {
        constraint->enableSpring(1, true);
        constraint->setStiffness(1, joint.spring_position[1]);
    }
    if(joint.spring_position[2] != 0) {
        constraint->enableSpring(2, true);
        constraint->setStiffness(2, -joint.spring_position[2]);
    }
    if(joint.spring_rotation[0] != 0) {
        constraint->enableSpring(3, true);
        constraint->setStiffness(3, joint.spring_rotation[0]);
    }
    if(joint.spring_rotation[1] != 0) {
        constraint->enableSpring(4, true);
        constraint->setStiffness(4, joint.spring_rotation[1]);
    }
    if(joint.spring_rotation[2] != 0) {
        constraint->enableSpring(5, true);
        constraint->setStiffness(5, joint.spring_rotation[2]);
    }

    this->constraint = std::move(constraint);
}

btTypedConstraint* MMDJoint::get_constraint() const {
    return this->constraint.get();
}