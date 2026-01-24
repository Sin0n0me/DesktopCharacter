#include "../../model/pmd/bone/IBoneAccessor.h"
#include "../../model/pmd/morph/IMorphAccessor.h"
#include "VMDMotion.h"
#include "VMDMotionManager.h"
#include <filesystem>

constexpr char8_t MOTION_WAVE_YUKARI[] = u8"assets/motion/手を振る.vmd";
constexpr char8_t MOTION_WAVE_AKARI[] = u8"assets/motion/手を振る_紲星あかり用.vmd";

VMDMotionManager::VMDMotionManager(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<IMorphAccessor>& morph_accessor
) {
    this->morph_accessor = morph_accessor;
    this->bone_accessor = bone_accessor;
    this->current_motion = MotionState::WaveHand;
}

bool VMDMotionManager::init() {
    this->motion_map.emplace(
        MotionState::WaveHand,
        VMDMotion{this->bone_accessor, this->morph_accessor}
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

    if(!motion.load_motion_file(MOTION_WAVE_YUKARI)) {
        return false;
    }

    return true;
}

void VMDMotionManager::update_motion(const DeltaTime& delta_time) {
    this->motion_map.at(this->current_motion).update_motion(delta_time);
}