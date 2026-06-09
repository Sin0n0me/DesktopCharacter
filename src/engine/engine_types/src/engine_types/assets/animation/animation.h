#pragma once
#include "../model/bone.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace enishi::types {
    // 1ボーンの1軸分のキーフレーム列
    template <typename T> struct Keyframes {
        std::vector<float> times; // 秒単位
        std::vector<T> values;
    };

    struct BoneTrack {
        BoneIndex bone_index;
        Keyframes<glm::vec3> positions;
        Keyframes<glm::quat> rotations;
        Keyframes<glm::vec3> scales;
    };

    struct AnimationClipData {
        std::string name;
        std::vector<BoneTrack> bone_tracks;
        float duration; // 秒
        bool is_looping;
    };

    struct BoneNode {
        glm::vec4 animation_rotate; // 四元数
        glm::vec4 ik_rotate;        // 四元数
    };
} // namespace enishi::types