#pragma once

#include "../VMDFileStruct.h"
#include "MorphKeyFrame.h"
#include "MorphKeyFrameCursor.h"
#include <map>
#include <memory>

class IMorphAccessor;

class MorphKeyFrameManager {
private:
    std::shared_ptr<IMorphAccessor> morph_accessor;
    std::vector<MorphKeyFrame> base_morph;
    std::map<uint32_t, std::unique_ptr<MorphKeyFrameCursor>> morph_key_frames;

public:

    explicit MorphKeyFrameManager(
        const std::shared_ptr<IMorphAccessor>& morph_accessor,
        const std::shared_ptr<KeyFrameTimer>& frame_manager,
        const std::vector<VMDMorphKeyFrame>& morph_key_frames
    );

    void apply_morph(void);

    KeyFrame get_last_frame(void) const noexcept;
};
