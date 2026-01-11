#pragma once
#include "../MotionState.h"
#include "../IMotion.h"
#include "VMDMotion.h"
#include <unordered_map>
#include <memory>

class IBoneAccessor;

class VMDMotionManager : public IMotion {
private:
	std::shared_ptr<IBoneAccessor> bone_accessor;
	std::unordered_map<MotionState, VMDMotion> motion_map;
	MotionState current_motion;
public:
	explicit VMDMotionManager(const std::shared_ptr<IBoneAccessor>& bone_accessor);

	bool init(void);

	bool load();

	void update_motion(const int64_t delta_time) override;
};
