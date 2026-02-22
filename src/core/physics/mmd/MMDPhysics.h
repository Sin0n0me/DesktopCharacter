#pragma once
#include"../Physics.h"
#include <memory>
#include <vector>

struct PMDRigidBody;
struct PMDPhysicsJoint;
class MMDPhysicsWorld;
class MMDRigidBody;
class MMDJoint;
class BoneNode;
class btOverlapFilterCallback;

class MMDPhysics : public Physics {
private:
    std::unique_ptr<btOverlapFilterCallback> filter_call_back;
    std::shared_ptr<MMDPhysicsWorld> mmd_world;

public:
    explicit MMDPhysics(
        const std::shared_ptr<MMDPhysicsWorld>& mmd_world
    );
    ~MMDPhysics(void) noexcept;

    bool is_initialized(void) const override;
    bool init(void) override;
    void reset_physics(void) override;
    void apply_physics(void) override;

    MMDPhysicsWorld* get_mmd_world(void) const;
};
