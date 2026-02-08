#pragma once
#include "../key_frame/KeyFrameCursor.h"
#include "MorphKeyFrame.h"

class IMorphAccessor;

class MorphKeyFrameCursor : public KeyFrameCursor<MorphKeyFrame> {
public:
    explicit MorphKeyFrameCursor(
        const std::shared_ptr<KeyFrameTimer>& frame_manager,
        std::vector<MorphKeyFrame>&& key_frame_list
    );

    void apply_offset(
        const uint32_t index,
        const std::shared_ptr<IMorphAccessor>& morph_accessor
    ) const;
};
