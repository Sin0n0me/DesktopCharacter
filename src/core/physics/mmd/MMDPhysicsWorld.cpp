#include "../../log/Logger.h"
#include "../../render/model/pmd/bone/BoneNode.h"
#include "../../render/model/pmd/bone/IBoneAccessor.h"
#include "../PhysicsSettings.h"
#include "../PhysicsWorld.h"
#include "../TransformConverter.h"
#include "joint/MMDJoint.h"
#include "MMDFilterCallback.h"
#include "MMDPhysicsWorld.h"
#include "motion_state/MMDMotionState.h"
#include "rigid_body/MMDRigidBody.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

MMDPhysicsWorld::MMDPhysicsWorld(
    const std::vector<std::shared_ptr<BoneNode>>& root_nodes
) :
    world(),
    root_nodes(root_nodes) {
}

MMDPhysicsWorld::~MMDPhysicsWorld(void) noexcept {
    this->destroy();
}

std::shared_ptr<MMDPhysicsWorld> MMDPhysicsWorld::make(
    const std::vector<std::shared_ptr<BoneNode>>& root_nodes
) {
    std::shared_ptr<MMDPhysicsWorld> physics(new MMDPhysicsWorld(root_nodes));

    if(!physics->init()) {
        return std::shared_ptr<MMDPhysicsWorld>();
    }

    return physics;
}

bool MMDPhysicsWorld::init(void) {
    this->world = std::make_shared<PhysicsWorld>(
        this->shared_from_this()
    );

    this->world->submit(
        [](btDiscreteDynamicsWorld* const world) {
            // TODO: 外から変更可能にする
            auto& info = world->getSolverInfo();
            info.m_numIterations = 10;
            info.m_solverMode = btSolverMode::SOLVER_SIMD;
        }
    );

    // 重力設定
    this->world->submit(
        [](btDiscreteDynamicsWorld* const world) {
            world->setGravity(btVector3(0, -GRAVITY * 2.0f, 0));
        }
    );

    // 地面作成
    this->make_ground();

    {
        auto filterCB = std::make_unique<MMDFilterCallback>();
        filterCB->add_proxy(this->ground_rigid_body->getBroadphaseProxy());
        this->filter_callback = std::move(filterCB);
    }
    this->world->submit(
        [this](btDiscreteDynamicsWorld* const world) {
            world->getPairCache()->setOverlapFilterCallback(
                this->filter_callback.get()
            );
        }
    );

    // TODO: マルチスレッドでボーン破壊なく動かすように
    // 1フレームずれることになるが前回フレームのグローバル行列を保持すれば
    // アニメーションやIKの適用中に物理演算をマルチスレッディングで動かせる
    this->world->set_synchronous(true);

    return true;
}

void MMDPhysicsWorld::start_simulation(void) {
    for(auto& rb : this->rigid_bodies) {
        rb->set_active(true);
    }
}

void MMDPhysicsWorld::finished_simulation(void) {
}

void MMDPhysicsWorld::reset_physics(btDiscreteDynamicsWorld* const world) {
    for(auto& rb : this->rigid_bodies) {
        rb->set_active(false);
        rb->reset_transform();
    }

    for(auto& rb : this->rigid_bodies) {
        rb->apply_global_transform();
    }
    for(auto& rb : this->rigid_bodies) {
        rb->apply_local_transform();
    }
    for(auto& node : this->root_nodes) {
        node->update_global();
    }
    for(auto& rb : this->rigid_bodies) {
        rb->reset(world);
    }
}

void MMDPhysicsWorld::apply_physics(btDiscreteDynamicsWorld* const world) {
    for(auto& rb : this->rigid_bodies) {
        rb->apply_global_transform();
    }
    for(auto& rb : this->rigid_bodies) {
        rb->apply_local_transform();
    }
    for(auto& node : this->root_nodes) {
        node->update_global();
    }
}

void MMDPhysicsWorld::make_ground(void) {
    this->ground_shape = std::make_unique<btStaticPlaneShape>(btVector3(0, 1, 0), 0.0f);
    this->ground_motion_state = std::make_unique<btDefaultMotionState>(btTransform::getIdentity());
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

bool MMDPhysicsWorld::add_rigid_body(
    const PMDRigidBody& rigid_body,
    const IBoneAccessor* bone_accessor,
    const std::shared_ptr<BoneNode>& node
) {
    if(!bool(this->world)) {
        return false;
    }

    auto mmd_rigid_body = MMDRigidBody::make_ptr(rigid_body, bone_accessor, node);
    if(!bool(mmd_rigid_body)) {
        return false;
    }

    // Bulletの世界に追加
    // ポインタのコピー(参照不可)
    const auto rb = mmd_rigid_body.get();
    this->world->submit(
        [rb](btDiscreteDynamicsWorld* const world) {
            world->addRigidBody(
                rb->get_rigid_body(),
                1 << rb->group,
                rb->group_mask
            );
        }
    );

    this->rigid_bodies.emplace_back(
        std::move(mmd_rigid_body)
    );

    return true;
}

bool MMDPhysicsWorld::add_joint(const PMDPhysicsJoint& joint) {
    if(!bool(this->world)) {
        return false;
    }

    const size_t size = this->rigid_bodies.size();
    const size_t rigid_body_a = joint.rigid_body_a;
    const size_t rigid_body_b = joint.rigid_body_b;
    // 0 index
    if(size < (rigid_body_a + 1) || size < (rigid_body_b + 1)) {
        return false;
    }
    if(rigid_body_a == rigid_body_b) {
        return false;
    }

    std::unique_ptr<MMDJoint> mmd_joint = std::make_unique<MMDJoint>(
        joint,
        this->rigid_bodies.at(rigid_body_a).get(),
        this->rigid_bodies.at(rigid_body_b).get()
    );

    // Bulletの世界に追加
    // ポインタのコピー(参照不可)
    const auto bt_joint = mmd_joint.get();
    this->world->submit(
        [bt_joint](btDiscreteDynamicsWorld* const world) {
            world->addConstraint(
                bt_joint->get_constraint()
            );
        }
    );

    this->joints.emplace_back(
        std::move(mmd_joint)
    );

    return true;
}

void MMDPhysicsWorld::destroy(void) {
    if(!bool(this->world)) {
        return;
    }

    for(const auto& joint : this->joints) {
        this->world->submit(
            [&joint](btDiscreteDynamicsWorld* const world) {
                world->removeConstraint(
                    joint->get_constraint()
                );
            }
        );
    }

    for(const auto& rigid_body : this->rigid_bodies) {
        this->world->submit(
            [&rigid_body](btDiscreteDynamicsWorld* const world) {
                world->removeRigidBody(
                    rigid_body->get_rigid_body()
                );
            }
        );
    }

    this->world->notify_finish();
}

void MMDPhysicsWorld::reset_physics(void) {
    this->world->reset_physics();
}

void MMDPhysicsWorld::apply_physics(void) {
    this->world->apply_physics();
}

void MMDPhysicsWorld::notify_update(const DeltaTime& delta_time) {
    this->world->notify_update(delta_time);
}