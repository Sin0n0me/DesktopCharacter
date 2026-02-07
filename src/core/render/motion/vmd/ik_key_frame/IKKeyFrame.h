#pragma once
#include "../../../model/pmd/bone/Bone.h"

struct IKKeyFrame {
    const BoneIndex index;
    const bool show_flag;

    explicit IKKeyFrame(
        const BoneIndex index,
        const bool show_flag
    ) noexcept;

    ~IKKeyFrame(void) noexcept = default;

    bool operator<(const IKKeyFrame& other) const;
};
