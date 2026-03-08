#include "../log/Logger.h"
#include "../timer/DeltaTime.h"
#include "IPhysicsSimulationListener.h"
#include "PhysicsSettings.h"
#include "PhysicsWorld.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

PhysicsWorld::PhysicsWorld(
    const std::shared_ptr<IPhysicsSimulationListener>& applier
) :
    physics_applier(applier) {
    this->is_running.store(false);
    this->do_physics_simulate.store(true);
    this->is_synchronous.store(false);
    this->updated_delta_time.store(false);
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
    std::lock_guard lock(this->flags_mutex);
    this->is_synchronous.store(flag);
    this->simulate_condition.notify_all();
}

void PhysicsWorld::set_fixed_time_step(const float time_step) {
    std::lock_guard lock(this->flags_mutex);
    this->fixed_time_step.store(time_step);
    this->simulate_condition.notify_all();
}

void PhysicsWorld::set_max_step_count(const int step_count) {
    std::lock_guard lock(this->flags_mutex);
    this->max_step_count.store(step_count);
    this->simulate_condition.notify_all();
}

void PhysicsWorld::notify_finish(void) {
    std::lock_guard lock(this->flags_mutex);
    this->is_running.store(false);
    this->simulate_condition.notify_all();
}

void PhysicsWorld::notify_update(const DeltaTime& delta_time) {
    {
        std::lock_guard lock(this->flags_mutex);
        this->delta_time = delta_time.to_float();
        this->updated_delta_time.store(true);
    }

    this->simulate_condition.notify_one(); // 物理シミュレーション開始の通知
}

void PhysicsWorld::submit(const Func& func) {
    if(!bool(this->world)) {
        return;
    }
    if(!this->is_running.load()) {
        return;
    }

    std::lock_guard lock(this->simulation_mutex);
    func(this->world.get());
}

void PhysicsWorld::reset_physics(void) {
    if(!bool(this->world)) {
        return;
    }

    if(const auto& applier = this->physics_applier.lock()) {
        std::lock_guard lock(this->simulation_mutex);
        applier->reset_physics(this->world.get());
    }
}

void PhysicsWorld::apply_physics(void) {
    if(!bool(this->world)) {
        return;
    }

    // 物理演算適用
    if(this->is_synchronous.load()) {
        std::lock_guard lock(this->flags_mutex);
        this->step_simulation();
    }

    if(const auto& applier = this->physics_applier.lock()) {
        std::lock_guard lock(this->simulation_mutex);
        applier->apply_physics(this->world.get());
    }
}

void PhysicsWorld::step_simulation(void) {
    std::lock_guard lock(this->simulation_mutex);
    if(!bool(this->world)) {
        return;
    }
    if(!this->is_running.load()) {
        return;
    }
    if(!this->do_physics_simulate.load()) {
        return;
    }

    // シミュレート開始通知
    if(const auto& applier = this->physics_applier.lock()) {
        applier->start_simulation();
    }

    // 物理シミュレート開始
    // デルタタイムがセットされずに呼び出された場合は固定時間を返す
    const bool is_update_enabled = this->updated_delta_time.load();
    const auto step_time = this->fixed_time_step.load();
    const auto max_step = this->max_step_count.load();
    const float delta_time = is_update_enabled ? this->delta_time.load() : step_time;
    this->updated_delta_time.store(false);

    this->world->stepSimulation(
        static_cast<btScalar>(delta_time),
        max_step,
        static_cast<btScalar>(step_time)
    );

    // シミュレート終了通知
    if(const auto& applier = this->physics_applier.lock()) {
        applier->finished_simulation();
    }
}

bool PhysicsWorld::wait_condition(void) const {
    // スレッド終了通知が来た場合
    // 他のフラグ関係なく待機終了
    if(!this->is_running.load()) {
        return true;
    }

    // 物理シミュレートが有効化されていなければ条件に関わらず待機
    if(!this->do_physics_simulate.load()) {
        return false;
    }

    // 同期時は別スレッド上で更新せずに待機
    if(this->is_synchronous.load()) {
        return false;
    }

    // デルタタイムがセットされたら待機終了
    return this->updated_delta_time.load();
}

void PhysicsWorld::simulation_update(void) {
    while(this->is_running.load()) {
        // 待機
        std::unique_lock<std::mutex> lock(this->flags_mutex);
        this->simulate_condition.wait(
            lock,
            [this]() -> bool {
                return this->wait_condition();
            }
        );

        // 物理シミュレート開始
        this->step_simulation();
    }
}