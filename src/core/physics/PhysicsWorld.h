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

class PhysicsWorld final {
public:
    using Func = std::function<void(btDiscreteDynamicsWorld* const)>;

private:
    std::weak_ptr<IPhysicsSimulationListener> physics_applier;
    std::unique_ptr<btDbvtBroadphase> broadphase;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDefaultCollisionConfiguration> collision_config;
    std::unique_ptr<btDiscreteDynamicsWorld> world;

    std::thread simulation_thread;
    std::atomic<float> delta_time;
    std::atomic<float> fixed_time_step;
    std::atomic<int> max_step_count;
    std::atomic<bool> is_running;
    std::atomic<bool> do_physics_simulate;
    std::atomic<bool> is_synchronous;
    std::atomic<bool> updated_delta_time;
    std::condition_variable simulate_condition;
    std::mutex simulation_mutex;
    std::mutex flags_mutex;

private:
    bool wait_condition(void) const;

    void simulation_update(void);

    void step_simulation(void);

public:
    explicit PhysicsWorld(
        const std::shared_ptr<IPhysicsSimulationListener>& applier
    );
    virtual ~PhysicsWorld(void) noexcept;

    /**
     * @brief 物理シミュレート用スレッドを終了通知を送る
     */
    void notify_finish(void);

    /**
     * @brief 物理シミュレート用スレッドに更新通知を送る
     * @param delta_time デルタタイム
     */
    void notify_update(const DeltaTime& delta_time);

    void submit(const Func& func);

    void reset_physics(void);

    void apply_physics(void);

    /**
     * @brief 物理シミュレートを同期させるかどうかのフラグをセットする
     * @param flag 同期させるならtrue 非同期ならfalse
     */
    void set_synchronous(const bool flag);

    void set_fixed_time_step(const float time_step);

    void set_max_step_count(const int step_count);
};
