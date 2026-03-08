#pragma once
#include "../IPhysicsSimulationListener.h"
#include <memory>
#include <vector>

struct btOverlapFilterCallback;
struct PMDRigidBody;
struct PMDPhysicsJoint;
class BoneNode;
class MMDRigidBody;
class MMDJoint;
class IBoneAccessor;
class PhysicsWorld;
class btCollisionShape;
class btMotionState;
class btRigidBody;
class DeltaTime;

class MMDPhysicsWorld : public std::enable_shared_from_this<MMDPhysicsWorld>, public IPhysicsSimulationListener {
private:
    std::shared_ptr<PhysicsWorld> world;
    std::unique_ptr<btOverlapFilterCallback> filter_callback;
    std::unique_ptr<btCollisionShape> ground_shape;
    std::unique_ptr<btMotionState> ground_motion_state;
    std::unique_ptr<btRigidBody> ground_rigid_body;
    std::vector<std::shared_ptr<BoneNode>> root_nodes;
    std::vector<std::unique_ptr<MMDRigidBody>> rigid_bodies;
    std::vector<std::unique_ptr<MMDJoint>> joints;

private:
    void start_simulation(void) override;
    void finished_simulation(void) override;
    void reset_physics(btDiscreteDynamicsWorld* const world) override;
    void apply_physics(btDiscreteDynamicsWorld* const world) override;

private:
    void make_ground(void);
    bool init(void);

private:
    explicit MMDPhysicsWorld(
        const std::vector<std::shared_ptr<BoneNode>>& root_nodes
    );

public:
    ~MMDPhysicsWorld(void) noexcept;

    static std::shared_ptr<MMDPhysicsWorld> make(
        const std::vector<std::shared_ptr<BoneNode>>& root_nodes
    );

    bool add_rigid_body(
        const PMDRigidBody& rigid_body,
        const IBoneAccessor* bone_accessor,
        const std::shared_ptr<BoneNode>& node
    );

    bool add_joint(const PMDPhysicsJoint& joint);

    void destroy(void);

    void reset_physics(void);

    void apply_physics(void);

    void notify_update(const DeltaTime& delta_time);
};
