#pragma once
#include "../key_frame/KeyFrameCursor.h"
#include "IKKeyFrame.h"

class IKKeyFrameCursor : public KeyFrameCursor<IKKeyFrame> {
public:
    explicit IKKeyFrameCursor(
        const std::shared_ptr<FrameManager>& frame_manager,
        std::vector<IKKeyFrame>&& key_frame_list
    );

    bool is_apply_ik(void);

    float progress(void) const;
};
