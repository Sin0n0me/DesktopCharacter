#pragma once

#include "../key_frame/KeyFrameCursor.h"
#include "BoneKeyFrame.h"
#include <DirectXMath.h>

class BoneKeyFrameCursor : public KeyFrameCursor<BoneKeyFrame> {
public:

    explicit BoneKeyFrameCursor(
        const std::shared_ptr<KeyFrameTimer>& frame_manager,
        std::vector<BoneKeyFrame>&& key_frame_list
    );

    // 線形補完済み
    DirectX::XMVECTOR get_rotate(void) const;
    DirectX::XMVECTOR get_translate(void) const;
};
