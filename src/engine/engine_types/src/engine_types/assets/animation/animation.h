#pragma once
#include "../model/bone.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace enishi::types {
    // 補間方式
    enum class InterpolationType : std::uint8_t {
        Linear,      // 線形補間
        Step,        // ステップ(即時切り替え)
        CubicSpline, // 三次スプライン
    };

    // 1ボーンの1軸分のキーフレーム列
    template <typename T> struct Keyframes {
        std::vector<float> times;        // 秒単位
        std::vector<T> values;           // 各フレーム毎の値
        std::vector<T> in_tangents;      // CubicSpline用
        std::vector<T> out_tangents;     // CubicSpline用
        InterpolationType interpolation; // 補完方式
    };

    struct BoneTrack {
        BoneIndex bone_index;
        Keyframes<glm::vec3> positions;
        Keyframes<glm::quat> rotations;
        Keyframes<glm::vec3> scales;
    };

    // モーフターゲットのトラック
    struct MorphTrack {
        std::uint32_t morph_index; // MorphTarget の添字
        Keyframes<float> weights;  // 0.0 〜 1.0
    };

    struct AnimationClipData {
        std::string name;
        std::vector<BoneTrack> bone_tracks;
        std::vector<MorphTrack> morph_tracks;
        float duration; // 秒
        bool is_looping;
    };
} // namespace enishi::types