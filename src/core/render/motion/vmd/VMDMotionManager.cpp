#include "../../model/pmd/IBoneAccessor.h"
#include "VMDMotion.h"
#include "VMDMotionManager.h"
#include <filesystem>
#include <iostream>

constexpr wchar_t MOTION_WAVE[] = L"assets/motion/手を振る.vmd";

VMDMotionManager::VMDMotionManager(const std::shared_ptr<IBoneAccessor>& bone_accessor) {
    this->bone_accessor = bone_accessor;
    this->current_motion = MotionState::WaveHand;
}

bool VMDMotionManager::init() {
    this->motion_map.emplace(
        MotionState::WaveHand,
        VMDMotion(this->bone_accessor)
    );

    if(!this->load()) {
        return false;
    }

    return true;
}

bool VMDMotionManager::load() {
    // 仮
    auto& motion = this->motion_map.at(this->current_motion);

    if(!motion.init_motion()) {
        return false;
    }

    if(!motion.load_motion_file(MOTION_WAVE)) {
        return false;
    }

    return true;
}

void VMDMotionManager::update_motion(const int64_t delta_time) {
    std::cout << delta_time << "micro second" << std::endl;

    this->motion_map.at(this->current_motion).update_motion(delta_time);
}