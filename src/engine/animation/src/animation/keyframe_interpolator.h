#pragma once
#include <engine_types/assets/animation/animation.h>
#include <glm/gtc/quaternion.hpp>

namespace enishi::animation {
    // フィールドは持たずに純粋なロジックのみを持つ
    class KeyframeInterpolator {
      public:
        template <typename T>
        [[nodiscard]]
        static T sample(const types::Keyframes<T>& keyframes, const float time) {
            // キーフレームが1つだけの場合
            if (keyframes.times.size() == 1) {
                return keyframes.values[0];
            }

            // 時刻が範囲外の場合はクランプ
            if (time <= keyframes.times.front()) {
                return keyframes.values.front();
            }
            if (time >= keyframes.times.back()) {
                return keyframes.values.back();
            }

            // time を挟む2つのキーフレームを二分探索で探す
            const auto it = std::lower_bound(keyframes.times.begin(), keyframes.times.end(), time);
            const std::uint32_t next_index =
                static_cast<std::uint32_t>(it - keyframes.times.begin());
            const std::uint32_t prev_index = next_index - 1;

            const float prev_time = keyframes.times[prev_index];
            const float next_time = keyframes.times[next_index];

            // 何かしらが原因で差がない場合は前回フレームの値を返す
            // ログで警告を出してもいいかも
            const float time_diff = next_time - prev_time;
            if (time_diff == 0.0) {
                return keyframes.values[prev_index];
            }

            // 区間内での正規化された時刻 [0.0, 1.0]
            const float t = (time - prev_time) / time_diff;

            switch (keyframes.interpolation) {
                case types::InterpolationType::Step:
                    return keyframes.values[prev_index];
                case types::InterpolationType::Linear:
                    return KeyframeInterpolator::lerp(
                        keyframes.values[prev_index], keyframes.values[next_index], t);
                case types::InterpolationType::CubicSpline:
                    return KeyframeInterpolator::cubic_spline(keyframes.values[prev_index],
                        keyframes.out_tangents[prev_index],
                        keyframes.values[next_index],
                        keyframes.in_tangents[next_index],
                        t,
                        next_time - prev_time);
            }

            return keyframes.values[prev_index];
        }

      private:
        // vec3 の線形補間
        [[nodiscard]]
        static constexpr glm::vec3 lerp(
            const glm::vec3& a, const glm::vec3& b, const float t) noexcept {
            return glm::mix(a, b, t);
        }

        // quat は slerp で補間(線形補間すると回転が歪む)
        [[nodiscard]]
        static glm::quat lerp(const glm::quat& a, const glm::quat& b, const float t) noexcept {
            return glm::slerp(a, b, t);
        }

        // float の線形補間(モーフウェイト用)
        [[nodiscard]]
        static float lerp(const float a, const float b, const float t) noexcept {
            return a + (b - a) * t;
        }

        // 三次エルミートスプライン補間
        template <typename T>
        [[nodiscard]]
        static constexpr T cubic_spline(const T& p0,
            const T& m0,
            const T& p1,
            const T& m1,
            const float t,
            const float dt) noexcept {
            const float t2 = t * t;
            const float t3 = t2 * t;
            // エルミート基底関数
            return (2.f * t3 - 3.f * t2 + 1.f) * p0 + (t3 - 2.f * t2 + t) * dt * m0 +
                   (-2.f * t3 + 3.f * t2) * p1 + (t3 - t2) * dt * m1;
        }
    };
} // namespace enishi::animation
