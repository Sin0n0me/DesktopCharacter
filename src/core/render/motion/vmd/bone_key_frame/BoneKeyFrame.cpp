#include "BoneKeyFrame.h"

BoneKeyFrame::BoneKeyFrame(const VMDBoneKeyFrame& key_frame) :
    frame(key_frame.frame),
    translation(
        DirectX::XMVectorSet(
            key_frame.translation[0],
            key_frame.translation[1],
            key_frame.translation[2],
            0.0f
        )
    ),
    rotation(
        DirectX::XMVectorSet(
            key_frame.rotation[0],
            key_frame.rotation[1],
            key_frame.rotation[2],
            key_frame.rotation[3]
        )
    ),
    bezier(
        VMDAnimationBezier(
            [&key_frame](void) -> std::array<std::uint8_t, 64> {
                std::array<std::uint8_t, 64> arr{};
                for(size_t i = 0; i < arr.size(); ++i) {
                    arr[i] = key_frame.interpolation[i];
                }
                return arr;
            } ()
                )
    ) {
}

BoneKeyFrame::BoneKeyFrame(const BoneKeyFrame& key_frame) :
    frame(key_frame.frame),
    translation(key_frame.translation),
    rotation(key_frame.rotation),
    bezier(key_frame.bezier) {
}

BoneKeyFrame::BoneKeyFrame(const BoneKeyFrame&& key_frame) :
    frame(key_frame.frame),
    translation(key_frame.translation),
    rotation(key_frame.rotation),
    bezier(key_frame.bezier) {
}

bool BoneKeyFrame::operator<(const BoneKeyFrame& other) const {
    return this->frame < other.frame;
}