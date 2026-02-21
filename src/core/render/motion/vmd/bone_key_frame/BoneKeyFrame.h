#pragma once
#include "../VMDFileStruct.h"
#include "VMDAnimationBezier.h"
#include <DirectXMath.h>

struct BoneKeyFrame {
    const DirectX::XMVECTOR translation;
    const DirectX::XMVECTOR rotation;
    const VMDAnimationBezier bezier;
    const uint32_t frame;

    explicit BoneKeyFrame(const VMDBoneKeyFrame& key_frame);
    explicit BoneKeyFrame(const BoneKeyFrame& key_frame);
    explicit BoneKeyFrame(const BoneKeyFrame&& key_frame);

    bool operator<(const BoneKeyFrame& other) const;
};
