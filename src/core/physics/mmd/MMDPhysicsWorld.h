#pragma once
#include "../IPhysicsSimulationListener.h"
#include "../PhysicsWorld.h"
#include <memory>
#include <vector>

struct PMDRigidBody;
struct PMDPhysicsJoint;
class BoneNode;
class MMDRigidBody;
class MMDJoint;

class MMDPhysicsWorld : public PhysicsWorld, public IPhysicsSimulationListener {
private:
    std::vector<std::shared_ptr<BoneNode>> root_nodes;
    std::vector<std::unique_ptr<MMDRigidBody>> rigid_bodies;
    std::vector<std::unique_ptr<MMDJoint>> joints;

private:
    void start_simulation(void) override;
    void finished_simulation(void) override;
    void reset_physics(btDiscreteDynamicsWorld* const world) override;
    void apply_physics(btDiscreteDynamicsWorld* const world) override;

public:
    explicit MMDPhysicsWorld(
        const std::vector<std::shared_ptr<BoneNode>>& root_nodes
    );

    bool add_rigid_body(
        const PMDRigidBody& rigid_body,
        const std::shared_ptr<BoneNode>& node
    );

    bool add_joint(const PMDPhysicsJoint& joint);
};
