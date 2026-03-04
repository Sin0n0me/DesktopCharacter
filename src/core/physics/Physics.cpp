#include "../timer/DeltaTime.h"
#include "Physics.h"
#include "PhysicsWorld.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

Physics::Physics(const std::shared_ptr<PhysicsWorld>& world) :
    world(world),
    ground_shape(std::make_unique<btStaticPlaneShape>(btVector3(0, 1, 0), 0.0f)),
    ground_motion_state(std::make_unique<btDefaultMotionState>(
        btTransform::getIdentity()
    )) {
    this->ground_rigid_body = std::make_unique<btRigidBody>(
        btRigidBody::btRigidBodyConstructionInfo(
            0,
            this->ground_motion_state.get(),
            this->ground_shape.get(),
            btVector3(0, 0, 0)
        )
    );

    this->world->submit(
        [this](btDiscreteDynamicsWorld* const world) {
            world->addRigidBody(this->ground_rigid_body.get());
        }
    );
}

void Physics::notify_update(const DeltaTime& delta_time) {
    this->world->notify_update(delta_time);
}