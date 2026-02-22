#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <thread>

class btBroadphaseInterface;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btDbvtBroadphase;
class btCollisionDispatcher;
class btRigidBody;
class DeltaTime;
class IPhysicsSimulationListener;

namespace {
    enum class SimulationState : int8_t {
        Waiting,
        Running,
        Finished
    };
}

class PhysicsWorld {
public:
    using Func = std::function<void(btDiscreteDynamicsWorld* const)>;

private:
    IPhysicsSimulationListener* const applier;
    std::unique_ptr<btDbvtBroadphase> broadphase;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDefaultCollisionConfiguration> collision_config;
    std::unique_ptr<btDiscreteDynamicsWorld> world;

    std::thread simulation_thread;
    std::optional<float> delta_time;
    std::atomic<float> fixed_time_step;
    std::atomic<int> max_step_count;
    std::atomic<bool> is_running;
    std::atomic<bool> do_physics_simulate;
    std::atomic<bool> is_synchronous;
    std::atomic<bool> sync_step_requested;
    std::condition_variable simulate_condition;
    std::condition_variable state_condition;
    std::mutex simulation_mutex;

private:
    void simulation_update(void);

protected:
    /**
     * @brief 物理シミュレート用スレッドを終了させる
     */
    void notify_finish(void);

    /**
     * @brief 物理シミュレートを同期させるかどうかのフラグをセットする
     * @param flag 同期させるならtrue 非同期ならfalse
     */
    void set_synchronous(const bool flag);

public:
    explicit PhysicsWorld(
        IPhysicsSimulationListener* const applier
    );
    virtual ~PhysicsWorld(void) noexcept;

    void notify_update(const DeltaTime& delta_time);
    void submit(const Func& func);
    void reset_physics(void);
    void apply_physics(void);
};
