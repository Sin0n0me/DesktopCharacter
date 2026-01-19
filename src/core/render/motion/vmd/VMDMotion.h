#pragma once

#include "../IMotionHandler.h"
#include "bone_key_frame/BoneKeyFrameManager.h"
#include "ik_key_frame/IKKeyFrameManager.h"
#include "morphkey_frame/MorphKeyFrameManager.h"

class FrameManager;
class IBoneAccessor;
class IMorphAccessor;

class VMDMotion : public IMotionHandler {
private:
    std::shared_ptr<FrameManager> frame_manager;
    std::shared_ptr<IBoneAccessor> bone_accessor;
    std::shared_ptr<IMorphAccessor> morph_accessor;
    std::unique_ptr<IKKeyFrameManager> ik;
    std::unique_ptr<MorphKeyFrameManager> morph_key_frame_manager;
    std::unique_ptr<BoneKeyFrameManager> bone_key_frame_manager;

public:
    explicit VMDMotion(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<IMorphAccessor>& morph_accessor
    );

    bool init_motion(void) override;
    bool load_motion_file(const std::filesystem::path& path) override;
    void update_motion(const DeltaTime& delta_time) override;
};
