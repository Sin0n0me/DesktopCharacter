#include "BoneKeyFrame.h"

BoneKeyFrame BoneKeyFrame::make(const VMDBoneKeyFrame& key_frame) {
    BoneKeyFrame frame{};
    frame.frame_index = key_frame.frame;
    frame.translation = DirectX::XMVectorSet(
        key_frame.translation[0],
        key_frame.translation[1],
        key_frame.translation[2],
        0.0f
    );
    frame.rotation = DirectX::XMVectorSet(
        key_frame.rotation[0],
        key_frame.rotation[1],
        key_frame.rotation[2],
        key_frame.rotation[3]
    );

    const float length = DirectX::XMVectorGetX(DirectX::XMVector4Length(frame.rotation));

    for(int i = 0; i < 64; ++i) {
        frame.interpolation[i] = key_frame.interpolation[i];
    }
    return frame;
}