#pragma once
#include "../IMotion.h"
#include "../MotionState.h"
#include "VMDMotion.h"
#include <memory>
#include <unordered_map>

class IBoneAccessor;
class IMorphAccessor;

class VMDMotionManager : public IMotion {
private:
    std::shared_ptr<IBoneAccessor> bone_accessor;
    std::shared_ptr<IMorphAccessor> morph_accessor;
    std::unordered_map<MotionState, VMDMotion> motion_map;
    MotionState current_motion;
public:
    explicit VMDMotionManager(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<IMorphAccessor>& morph_accessor
    );

    bool init(void);

    bool load();

    void update_motion(const DeltaTime& delta_time) override;
};
