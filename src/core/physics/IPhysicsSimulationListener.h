#pragma once

class btDiscreteDynamicsWorld;

class IPhysicsSimulationListener {
public:
    virtual ~IPhysicsSimulationListener(void) noexcept = default;

    virtual void start_simulation(void) = 0;

    virtual void finished_simulation(void) = 0;

    virtual void reset_physics(btDiscreteDynamicsWorld* const world) = 0;

    virtual void apply_physics(btDiscreteDynamicsWorld* const world) = 0;
};
