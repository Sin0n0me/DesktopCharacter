#pragma once
#include "../VMDFileStruct.h"
#include <DirectXMath.h>

struct BoneKeyFrame {
    DirectX::XMVECTOR translation;
    DirectX::XMVECTOR rotation;
    uint8_t interpolation[64];
    uint32_t frame_index;

    static BoneKeyFrame make(const VMDBoneKeyFrame& key_frame);
};
