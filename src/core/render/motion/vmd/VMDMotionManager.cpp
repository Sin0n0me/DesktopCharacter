#include "../../model/pmd/bone/IBoneAccessor.h"
#include "../../model/pmd/morph/IMorphAccessor.h"
#include "VMDMotion.h"
#include "VMDMotionManager.h"
#include <filesystem>

constexpr char8_t MOTION_WAVE_YUKARI[] = u8"assets/motion/手を振る.vmd";
constexpr char8_t MOTION_WALK_YUKARI[] = u8"assets/motion/歩く_結月ゆかり用.vmd";
constexpr char8_t MOTION_WAVE_AKARI[] = u8"assets/motion/手を振る_紲星あかり用.vmd";

VMDMotionManager::VMDMotionManager(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<IMorphAccessor>& morph_accessor,
    const std::shared_ptr<IKSolver>& ik_solver,
    const std::shared_ptr<MMDPhysics>& physics
) :
    morph_accessor(morph_accessor),
    bone_accessor(bone_accessor),
    ik_solver(ik_solver),
    physics(physics),
    current_motion(MotionState::WaveHand) {
}

bool VMDMotionManager::init(void) {
    this->motion_map.emplace(
        MotionState::WaveHand,
        new VMDMotion(
            this->bone_accessor,
            this->morph_accessor,
            this->ik_solver,
            this->physics
        )
    );

    if(!this->load()) {
        return false;
    }

    return true;
}

bool VMDMotionManager::load() {
    // 仮
    auto& motion = this->motion_map.at(this->current_motion);
    const std::filesystem::path path(MOTION_WALK_YUKARI);

    if(!motion->load_motion_file(path)) {
        return false;
    }

    if(!motion->init_motion()) {
        return false;
    }

    return true;
}

void VMDMotionManager::update_motion(const DeltaTime& delta_time) {
    this->motion_map.at(this->current_motion)->update_motion(delta_time);
}