#include "BoneKeyFrame.h"

BoneKeyFrame::BoneKeyFrame(const VMDBoneKeyFrame& key_frame) :
    frame(key_frame.frame),
    translation(DirectX::XMVectorSet(
        key_frame.translation[0],
        key_frame.translation[1],
        key_frame.translation[2],
        0.0f
    )),
    rotation(DirectX::XMVectorSet(
        key_frame.rotation[0],
        key_frame.rotation[1],
        key_frame.rotation[2],
        key_frame.rotation[3]
    )),
    interpolation({}) {
    for(int i = 0; i < sizeof(decltype(this->interpolation)); ++i) {
        *const_cast<uint8_t*>(&this->interpolation[i]) = key_frame.interpolation[i];
    }
}

BoneKeyFrame::BoneKeyFrame(const BoneKeyFrame& key_frame) :
    frame(key_frame.frame),
    translation(key_frame.translation),
    rotation(key_frame.rotation),
    interpolation(key_frame.interpolation) {
}

BoneKeyFrame::BoneKeyFrame(const BoneKeyFrame&& key_frame) :
    frame(key_frame.frame),
    translation(key_frame.translation),
    rotation(key_frame.rotation),
    interpolation(key_frame.interpolation) {
}

bool BoneKeyFrame::operator<(const BoneKeyFrame& other) const {
    return this->frame < other.frame;
}