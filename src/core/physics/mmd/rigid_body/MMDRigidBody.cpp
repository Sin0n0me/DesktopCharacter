#include "../../../log/Logger.h"
#include "../../../render/model/pmd/bone/BoneNode.h"
#include "../../../utility/Hash.h"
#include "../MMDPhysics.h"
#include "../motion_state/MMDDynamicAndBoneMergeMotionState.h"
#include "../motion_state/MMDDynamicMotionState.h"
#include "../motion_state/MMDKinematicMotionState.h"
#include "../motion_state/MMDMotionState.h"
#include "MMDRigidBody.h"
#include "MMDRigidBodyShape.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

MMDRigidBody::MMDRigidBody(
    const PMDRigidBody& rigid_body,
    const std::shared_ptr<BoneNode>& node
) :
    node(node),
    group(rigid_body.group_index),
    group_mask(rigid_body.group_target),
    rigid_body_type(get_mmd_rigid_body_type(rigid_body.rigid_body_type)),
    name_hash(hash_u32(rigid_body.name)),
    shape(MMDRigidBody::make_shape(rigid_body)),
    active_motion_state(),
    kinematic_motion_state(),
    rigid_body() {
}

std::unique_ptr<btCollisionShape> MMDRigidBody::make_shape(const PMDRigidBody& rigid_body) {
    switch(get_mmd_rigid_body_shape(rigid_body.shape_type)) {
    case MMDRigidBodyShape::Sphere:
        return std::make_unique<btSphereShape>(
            rigid_body.shape_size[0]
        );
    case MMDRigidBodyShape::Box:
        return std::make_unique<btBoxShape>(
            btVector3(
                rigid_body.shape_size[0],
                rigid_body.shape_size[1],
                rigid_body.shape_size[2]
            )
        );
    case MMDRigidBodyShape::Capsule:
        return std::make_unique<btCapsuleShape>(
            rigid_body.shape_size[0],
            rigid_body.shape_size[1]
        );
    default:
        break;
    }

    return std::unique_ptr<btCollisionShape>();
}

// 剛体中心とのオフセットは以下で求める(列優先の場合)
// Inverse(global) * InvZ(InvZ(global) * T * R)
MMDMatrix MMDRigidBody::make_offset(
    const PMDRigidBody& rigid_body,
    const BoneNode* node
) {
    // 剛体自体のオフセットを
    // pitch, yaw, rollの順序で格納されている(Z->X->Yの順序になるよう)
    // 引数はroll, pitch, yawの順序
    const MMDMatrix global = node->get_global();
    const BulletMatrix rotate_matrix = BulletMatrix::make_rotation_from_roll_pitch_yaw(
        Radian(rigid_body.rotation[2]),
        Radian(rigid_body.rotation[0]),
        Radian(rigid_body.rotation[1])
    );
    const BulletMatrix translate_matrix = BulletMatrix::make_translation(
        rigid_body.position[0],
        rigid_body.position[1],
        rigid_body.position[2]
    );
    const BulletMatrix transform = translate_matrix * rotate_matrix;
    const BulletMatrix world_transform = global.to_bullet() * transform;

    // to_mmd()で行優先へ転置し右手系から左手系へ変換
    return world_transform.to_mmd() * global.inverse();
}

bool MMDRigidBody::make_rigid_body(const PMDRigidBody& rigid_body) {
    if(!bool(this->shape)) {
        return false;
    }

    const bool is_static = this->rigid_body_type == MMDRigidBodyType::Static;
    const btScalar mass = is_static ? 0.0f : rigid_body.mass;
    btVector3 local_interia(0, 0, 0);

    if(mass != 0.0f) {
        this->shape->calculateLocalInertia(mass, local_interia);
    }

    if(!this->make_motion_state(rigid_body)) {
        return false;
    }

    btMotionState* const motion_state = is_static ?
        this->kinematic_motion_state.get() :
        this->active_motion_state.get();
    btRigidBody::btRigidBodyConstructionInfo construct_info(
        mass,
        motion_state,
        this->shape.get(),
        local_interia
    );
    construct_info.m_linearDamping = rigid_body.linear_damping;
    construct_info.m_angularDamping = rigid_body.angular_damping;
    construct_info.m_restitution = rigid_body.restitution;
    construct_info.m_friction = rigid_body.friction;
    construct_info.m_additionalDamping = true;

    this->rigid_body = std::make_unique<btRigidBody>(construct_info);
    this->rigid_body->setSleepingThresholds(0.01f, Degree(0.1f).to_radian_value());
    this->rigid_body->setUserPointer(this);
    this->rigid_body->setActivationState(DISABLE_DEACTIVATION);

    if(is_static) {
        this->rigid_body->setCollisionFlags(
            this->rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT
        );
    }

    return true;
}

bool MMDRigidBody::make_motion_state(const PMDRigidBody& rigid_body) {
    const MMDMatrix offset = this->make_offset(rigid_body, this->node.get());

    switch(this->rigid_body_type) {
    case MMDRigidBodyType::Static:
        this->kinematic_motion_state = std::make_unique<MMDKinematicMotionState>(
            this->node,
            offset
        );
        break;
    case MMDRigidBodyType::Dynamic:
        this->active_motion_state = std::make_unique<MMDDynamicMotionState>(
            this->node,
            offset
        );
        this->kinematic_motion_state = std::make_unique<MMDKinematicMotionState>(
            this->node,
            offset
        );
        break;
    case MMDRigidBodyType::DynamicAdjustBone:
        this->active_motion_state = std::make_unique<MMDDynamicAndBoneMergeMotionState>(
            this->node,
            offset
        );
        this->kinematic_motion_state = std::make_unique<MMDKinematicMotionState>(
            this->node,
            offset
        );
        break;
    default:
        return false;
    }

    return true;
}

bool MMDRigidBody::init(const PMDRigidBody& rigid_body) {
    if(!this->make_rigid_body(rigid_body)) {
        return false;
    }

    return true;
}

std::unique_ptr<MMDRigidBody> MMDRigidBody::make_ptr(
    const PMDRigidBody& rigid_body,
    const std::shared_ptr<BoneNode>& node
) {
    auto mmd_rigid_body = std::unique_ptr<MMDRigidBody>(
        new MMDRigidBody(rigid_body, node) // privateコンストラクタなので
    );
    if(!mmd_rigid_body->init(rigid_body)) {
        return std::unique_ptr<MMDRigidBody>();
    }

    return std::move(mmd_rigid_body);
}

btRigidBody* MMDRigidBody::get_rigid_body(void) const {
    return this->rigid_body.get();
}

void MMDRigidBody::set_active(const bool active_flag) {
    const auto current_flag = this->rigid_body->getCollisionFlags();
    if(this->rigid_body_type != MMDRigidBodyType::Static) {
        const auto motion_state = active_flag ?
            this->active_motion_state.get() :
            this->kinematic_motion_state.get();

        const auto flag = current_flag & ~btCollisionObject::CF_KINEMATIC_OBJECT;
        this->rigid_body->setCollisionFlags(flag);
        this->rigid_body->setMotionState(motion_state);
    } else {
        const auto flag = current_flag | btCollisionObject::CF_KINEMATIC_OBJECT;
        this->rigid_body->setCollisionFlags(flag);
        this->rigid_body->setMotionState(
            this->kinematic_motion_state.get()
        );
    }
}

void MMDRigidBody::reset(btDiscreteDynamicsWorld* const world) {
    const auto cache = world->getPairCache();
    if(cache != nullptr) {
        cache->cleanProxyFromPairs(
            this->rigid_body->getBroadphaseHandle(),
            world->getDispatcher()
        );
    }
    this->rigid_body->setAngularVelocity(btVector3(0, 0, 0));
    this->rigid_body->setLinearVelocity(btVector3(0, 0, 0));
    this->rigid_body->clearForces();
}

void MMDRigidBody::reset_transform(void) {
    if(bool(this->active_motion_state)) {
        this->active_motion_state->reset();
    }
}

void MMDRigidBody::apply_local_transform(void) {
    if(!bool(this->node)) {
        return;
    }

    // 物理状態の反映(ローカル空間)
    const auto& global = this->node->get_global();
    if(const auto parent_node = this->node->parent.lock()) {
        const MMDMatrix& parent_globale = parent_node->get_global();
        const MMDMatrix& local = global * parent_globale.inverse();

        this->node->set_local(local);
    } else {
        this->node->set_local(global);
    }
}

void MMDRigidBody::apply_global_transform(void) {
    if(bool(this->active_motion_state)) {
        this->active_motion_state->reflect_global_transform();
    }
    if(bool(this->kinematic_motion_state)) {
        this->kinematic_motion_state->reflect_global_transform();
    }
}