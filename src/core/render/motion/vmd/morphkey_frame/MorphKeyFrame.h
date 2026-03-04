#pragma once
#include "../key_frame/KeyFrame.h"
#include "../VMDFileStruct.h"

struct MorphKeyFrame {
    const KeyFrame frame;
    const float weight;

    explicit MorphKeyFrame(const VMDMorphKeyFrame& key_frame);

    bool operator<(const MorphKeyFrame& other) const;
};
