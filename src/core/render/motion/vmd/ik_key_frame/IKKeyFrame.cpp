#include "IKKeyFrame.h"

IKKeyFrame::IKKeyFrame(const BoneIndex index, const bool show_flag) noexcept :
    index(index),
    show_flag(show_flag) {
}

bool IKKeyFrame::operator<(const IKKeyFrame& other) const {
    return this->index < other.index;
}