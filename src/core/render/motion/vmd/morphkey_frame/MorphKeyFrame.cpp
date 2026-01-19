#include "MorphKeyFrame.h"

MorphKeyFrame::MorphKeyFrame(const VMDMorphKeyFrame& key_frame) :
    frame(key_frame.frame),
    weight(key_frame.weight) {
}

bool MorphKeyFrame::operator<(const MorphKeyFrame& other) const {
    return this->frame < other.frame;
}