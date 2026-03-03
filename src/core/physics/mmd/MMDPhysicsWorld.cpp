#include "../../log/Logger.h"
#include "../../render/model/pmd/bone/BoneNode.h"
#include "../PhysicsSettings.h"
#include "../TransformConverter.h"
#include "joint/MMDJoint.h"
#include "MMDPhysicsWorld.h"
#include "motion_state/MMDMotionState.h"
#include "rigid_body/MMDRigidBody.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

MMDPhysicsWorld::MMDPhysicsWorld(
    const std::vector<std::shared_ptr<BoneNode>>& root_nodes
) :
    PhysicsWorld(this),
    root_nodes(root_nodes) {
    this->submit(
        [](btDiscreteDynamicsWorld* const world) {
            // TODO: 外から変更可能にする:w
            auto& info = world->getSolverInfo();
            info.m_numIterations = 10;
            info.m_solverMode = btSolverMode::SOLVER_SIMD;
        }
    );

    // 重力設定
    this->submit(
        [](btDiscreteDynamicsWorld* const world) {
            world->setGravity(btVector3(0, -GRAVITY * 1.0f, 0));
        }
    );

    // TODO: マルチスレッドでボーン破壊なく動かすように
    // 1フレームずれることになるが前回フレームのグローバル行列を保持すれば
    // アニメーションやIKの適用中に物理演算をマルチスレッディングで動かせる
    this->set_synchronous(true);
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
        // node->update_global();
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
        //node->update_global();
    }
}

bool MMDPhysicsWorld::add_rigid_body(
    const PMDRigidBody& rigid_body,
    const std::shared_ptr<BoneNode>& node
) {
    auto mmd_rigid_body = MMDRigidBody::make_ptr(rigid_body, node);
    if(!bool(mmd_rigid_body)) {
        return false;
    }

    // Bulletの世界に追加
    // ポインタのコピー(参照不可)
    const auto rb = mmd_rigid_body.get();
    this->submit(
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
    const auto size = this->rigid_bodies.size();
    // 0 index
    if(size < joint.rigid_body_a || size < joint.rigid_body_b) {
        return false;
    }

    std::unique_ptr<MMDJoint> mmd_joint = std::make_unique<MMDJoint>(
        joint,
        this->rigid_bodies[joint.rigid_body_a].get(),
        this->rigid_bodies[joint.rigid_body_b].get()
    );

    // Bulletの世界に追加
    // ポインタのコピー(参照不可)
    const auto bt_joint = mmd_joint.get();
    this->submit(
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