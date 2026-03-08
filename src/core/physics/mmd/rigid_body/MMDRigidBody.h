#pragma once
#include "../../../math/WrappedMatrix.h"
#include "MMDRigidBodyType.h"
#include <memory>

struct PMDRigidBody;
class btRigidBody;
class btCollisionShape;
class MMDMotionState;
class BoneNode;
class MMDPhysics;
class btDiscreteDynamicsWorld;
class IBoneAccessor;

class MMDRigidBody {
private:
    std::unique_ptr<btCollisionShape> shape;
    std::unique_ptr<MMDMotionState>	active_motion_state;
    std::unique_ptr<MMDMotionState>	kinematic_motion_state;
    std::unique_ptr<btRigidBody> rigid_body;
    const std::shared_ptr<BoneNode> node;

public:
    const uint32_t name_hash;
    const uint16_t group;
    const uint16_t group_mask;
    const MMDRigidBodyType rigid_body_type;

private:
    explicit MMDRigidBody(void) = delete;

    static std::unique_ptr<btCollisionShape> make_shape(const PMDRigidBody& rigid_body);
    static MMDMatrix make_offset_from_pmd(const PMDRigidBody& rigid_body, const std::shared_ptr<BoneNode>& node);
    bool make_rigid_body(const PMDRigidBody& rigid_body, const IBoneAccessor* bone_accessor);
    bool make_motion_state(const PMDRigidBody& rigid_body, const IBoneAccessor* bone_accessor);

protected:
    explicit MMDRigidBody(
        const PMDRigidBody& rigid_body,
        const std::shared_ptr<BoneNode>& node
    );

    bool init(
        const PMDRigidBody& rigid_body,
        const IBoneAccessor* bone_accessor
    );

public:

    static std::unique_ptr<MMDRigidBody> make_ptr(
        const PMDRigidBody& rigid_body,
        const IBoneAccessor* bone_accessor,
        const std::shared_ptr<BoneNode>& node
    );

    btRigidBody* get_rigid_body(void) const;

    void set_active(const bool flag);
    void reset(btDiscreteDynamicsWorld* const world);
    void reset_transform(void);
    void apply_local_transform(void);
    void apply_global_transform(void);
};
