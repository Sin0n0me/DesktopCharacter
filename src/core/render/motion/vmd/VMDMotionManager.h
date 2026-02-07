#pragma once
#include "../IMotion.h"
#include "../MotionState.h"
#include <memory>
#include <unordered_map>

class IBoneAccessor;
class IMorphAccessor;
class VMDMotion;
class IKSolver;

class VMDMotionManager : public IMotion {
private:
    std::shared_ptr<IBoneAccessor> bone_accessor;
    std::shared_ptr<IMorphAccessor> morph_accessor;
    std::shared_ptr<IKSolver> ik_solver;
    std::unordered_map<MotionState, std::unique_ptr<VMDMotion>> motion_map;
    MotionState current_motion;
public:
    explicit VMDMotionManager(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<IMorphAccessor>& morph_accessor,
        const std::shared_ptr<IKSolver>& ik_solver
    );

    bool init(void);

    bool load();

    void update_motion(const DeltaTime& delta_time) override;
};
