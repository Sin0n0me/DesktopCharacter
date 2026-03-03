#include "../log/Logger.h"
#include "../timer/DeltaTime.h"
#include "IPhysicsSimulationListener.h"
#include "PhysicsSettings.h"
#include "PhysicsWorld.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

PhysicsWorld::PhysicsWorld(
    IPhysicsSimulationListener* const applier
) :
    applier(applier) {
    this->is_running.store(false);
    this->do_physics_simulate.store(true);
    this->is_synchronous.store(false);
    this->sync_step_requested.store(false);
    this->max_step_count.store(2);
    this->fixed_time_step.store(1.0f / 60.0f);

    try {
        this->broadphase = std::make_unique<btDbvtBroadphase>();
        this->collision_config = std::make_unique<btDefaultCollisionConfiguration>();
        this->solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        this->dispatcher = std::make_unique<btCollisionDispatcher>(
            this->collision_config.get()
        );

        this->world = std::make_unique<btDiscreteDynamicsWorld>(
            this->dispatcher.get(),
            this->broadphase.get(),
            this->solver.get(),
            this->collision_config.get()
        );
        this->world->setGravity(btVector3(0, -GRAVITY, 0));

        this->is_running.store(true);

        this->simulation_thread = std::thread(
            &PhysicsWorld::simulation_update,
            this
        );
    } catch(const std::exception& e) {
        const std::string err(e.what());
        Logger::error(
            std::u8string(err.begin(), err.end())
        );
    }
}

PhysicsWorld::~PhysicsWorld(void) noexcept {
    this->notify_finish();
    if(this->simulation_thread.joinable()) {
        this->simulation_thread.join();
    }
}

void PhysicsWorld::set_synchronous(const bool flag) {
    std::lock_guard lock(this->simulation_mutex);
    this->is_synchronous.store(flag);
    this->simulate_condition.notify_all();
}

void PhysicsWorld::notify_finish(void) {
    this->is_running.store(false);
    this->simulate_condition.notify_all();
    this->state_condition.notify_all();
}

void PhysicsWorld::notify_update(const DeltaTime& delta_time) {
    {
        std::lock_guard lock(this->simulation_mutex);
        this->delta_time = delta_time.to_float();
    }

    this->simulate_condition.notify_one(); // 物理シミュレーション開始の通知
}

void PhysicsWorld::submit(const Func& func) {
    if(!bool(this->world)) {
        return;
    }

    std::lock_guard lock(this->simulation_mutex);
    func(this->world.get());
}

void PhysicsWorld::reset_physics(void) {
    if(!bool(this->world)) {
        return;
    }

    std::lock_guard lock(this->simulation_mutex);
    this->applier->reset_physics(this->world.get());
}

void PhysicsWorld::apply_physics(void) {
    if(!bool(this->world)) {
        return;
    }

    // 同期時はこの場でシミュレート待機
    if(this->is_synchronous.load()) {
        std::unique_lock lock(this->simulation_mutex);
        this->sync_step_requested.store(true);
        this->simulate_condition.notify_one();
        this->state_condition.wait(
            lock,
            [this] {
                return !this->sync_step_requested.load()
                    || !this->is_running.load();
            }
        );
    }

    // 物理演算適用
    std::lock_guard lock(this->simulation_mutex);
    this->applier->apply_physics(this->world.get());
}

void PhysicsWorld::simulation_update(void) {
    while(this->is_running.load()) {
        // 待機
        std::unique_lock<std::mutex> lock(this->simulation_mutex);
        const auto wait = [this](void) -> bool {
            // このスレッド終了通知が来た場合
            // 他のフラグ関係なく待機終了
            if(!this->is_running.load()) {
                return true;
            }

            // 物理シミュレートが有効化されていなければ条件に関わらず待機
            if(!this->do_physics_simulate.load()) {
                return false;
            }

            // 同期時かつ待機解除時
            if(this->is_synchronous.load() && this->sync_step_requested.load()) {
                return true;
            }

            // 非同期時かつデルタタイムがセットされた場合
            if(!this->is_synchronous.load() && this->delta_time.has_value()) {
                return true;
            }

            return false;
            };
        this->simulate_condition.wait(
            lock,
            wait
        );

        // シミュレート開始通知
        this->applier->start_simulation();

        // 物理シミュレート開始
        const auto step_time = this->fixed_time_step.load();
        const auto dt = this->delta_time.value_or(step_time);
        const auto max_step = this->max_step_count.load();
        this->delta_time.reset();
        this->world->stepSimulation(
            static_cast<btScalar>(dt),
            max_step,
            static_cast<btScalar>(step_time)
        );

        // シミュレート終了通知
        this->applier->finished_simulation();
        this->sync_step_requested.store(false);
        this->state_condition.notify_all();
    }
}