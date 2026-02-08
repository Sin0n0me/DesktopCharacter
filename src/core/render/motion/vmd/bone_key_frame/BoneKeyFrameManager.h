#pragma once

#include "../../../model/pmd/bone/Bone.h"
#include "BoneKeyFrameCursor.h"
#include <map>
#include <memory>
#include <vector>

class IBoneAccessor;

class BoneKeyFrameManager {
    const std::shared_ptr<const IBoneAccessor> bone_accessor;
    std::map<BoneIndex, std::unique_ptr<BoneKeyFrameCursor>> bone_key_frame_map;

public:
    explicit BoneKeyFrameManager(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<KeyFrameTimer>& frame_manager,
        const std::vector<VMDBoneKeyFrame>& key_frame_list
    );

    void update_local_matricies(void);

    void update_global_matricies(void);

    void apply_skinning(void);

    KeyFrame get_last_frame(void) const noexcept;
};
