#pragma once

#include "../IMotionHandler.h"

class KeyFrameTimer;
class IBoneAccessor;
class IMorphAccessor;
class MorphKeyFrameManager;
class IKKeyFrameManager;
class BoneKeyFrameManager;
class IKSolver;
class MMDPhysics;

class VMDMotion : public IMotionHandler {
private:
    const std::shared_ptr<KeyFrameTimer> frame_manager;
    const std::shared_ptr<IBoneAccessor> bone_accessor;
    const std::shared_ptr<IMorphAccessor> morph_accessor;
    const std::shared_ptr<IKSolver> ik_solver;
    const std::shared_ptr<MMDPhysics> physics;
    std::unique_ptr<IKKeyFrameManager> ik_key_frame_manager;
    std::unique_ptr<MorphKeyFrameManager> morph_key_frame_manager;
    std::unique_ptr<BoneKeyFrameManager> bone_key_frame_manager;

public:
    explicit VMDMotion(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<IMorphAccessor>& morph_accessor,
        const std::shared_ptr<IKSolver>& ik_solver,
        const std::shared_ptr<MMDPhysics>& physics
    );

    bool init_motion(void) override;
    bool load_motion_file(const std::filesystem::path& path) override;
    void update_motion(const DeltaTime& delta_time) override;
};
