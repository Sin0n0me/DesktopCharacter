#pragma once
#include <memory>

class btCollisionShape;
class btMotionState;
class btRigidBody;
class PhysicsWorld;
class DeltaTime;

class Physics {
protected:
    std::unique_ptr<btCollisionShape> ground_shape;
    std::unique_ptr<btMotionState> ground_motion_state;
    std::unique_ptr<btRigidBody> ground_rigid_body;
    std::shared_ptr<PhysicsWorld> world;

public:
    explicit Physics(const std::shared_ptr<PhysicsWorld>& world);
    virtual ~Physics(void) noexcept = default;

    virtual bool is_initialized(void) const = 0;

    virtual bool init(void) = 0;

    virtual void reset_physics(void) = 0;

    virtual void apply_physics(void) = 0;

    void notify_update(const DeltaTime& delta_time);
};
