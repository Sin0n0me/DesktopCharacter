#pragma once
#include "../model/bone.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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

    struct AnimationRotate {
        glm::quat rotate;
    };

    struct IKLimit {
        glm::vec3 axis; // 回転を制限する軸
        float limit;
    };

    struct IKRotate {
        glm::quat rotate;
    };

    struct AnimationFlags {
        bool apply_animation;
        bool apply_ik;
        bool apply_limit_ik;
        bool apply_physics;
    };
} // namespace enishi::types