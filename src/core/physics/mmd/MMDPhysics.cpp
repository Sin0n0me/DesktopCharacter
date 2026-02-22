#include "../../render/model/pmd/bone/BoneNode.h"
#include "../../timer/DeltaTime.h"
#include "../Physics.h"
#include "../PhysicsWorld.h"
#include "MMDFilterCallback.h"
#include "MMDPhysics.h"
#include "MMDPhysicsWorld.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

MMDPhysics::MMDPhysics(
    const std::shared_ptr<MMDPhysicsWorld>& mmd_world
) :
    Physics(mmd_world),
    mmd_world(mmd_world) {
}

MMDPhysics::~MMDPhysics(void) noexcept {
}

bool MMDPhysics::is_initialized(void) const {
    return bool(this->filter_call_back);
}

bool MMDPhysics::init(void) {
    {
        auto filterCB = std::make_unique<MMDFilterCallback>();
        filterCB->add_proxy(this->ground_rigid_body->getBroadphaseProxy());
        this->filter_call_back = std::move(filterCB);
    }

    this->world->submit(
        [this](btDiscreteDynamicsWorld* const world) {
            world->getPairCache()->setOverlapFilterCallback(
                this->filter_call_back.get()
            );
        }
    );

    return true;
}

void MMDPhysics::reset_physics(void) {
    this->world->reset_physics();
    this->world->notify_update(DeltaTime(1.0f / 60.0f));
}

void MMDPhysics::apply_physics(void) {
    this->world->apply_physics();
}

MMDPhysicsWorld* MMDPhysics::get_mmd_world(void) const {
    return this->mmd_world.get();
}