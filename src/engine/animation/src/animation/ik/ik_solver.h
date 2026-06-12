#pragma once
#include <engine_types/assets/animation/animation.h>
#include <engine_types/assets/model/bone.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <set>
#include <unordered_map>

namespace enishi::animation {
    class IKSolver {
      private:
        std::unordered_map<types::BoneIndex, PMDIK> ik_map; // first: index, second: ik
        std::unordered_map<types::BoneIndex, glm::quat>
            ik_quaternion_map; // first: index, second: ik(quaternion)
        std::unordered_map<types::BoneIndex, glm::vec4>
            hinge_map; // first: index, second: ヒンジ軸 膝などヒンジ関節用

      protected:
        static glm::quat decompose_swing_twist(const glm::quat& q,
            const glm::vec3& twist_axis,
            const float twist_min,
            const float twist_max,
            const float swing_max);

        static glm::quat clamp_swing_cone(
            const glm::quat& swing, const glm::vec3& twist_axis, const float max);

        static glm::quat quaternion_from_to(const glm::quat& from, const glm::quat& to);

      protected:
        void solve_ik_bone(const BoneIndex& index,
            BoneNode* const bone_node,
            const BoneNode* ik_bone_node,
            const BoneNode* target_bone_node,
            const float& ik_limit) const;

      public:
        explicit IKSolver(const std::shared_ptr<IBoneAccessor>& bone_accessor,
            const std::shared_ptr<const PMDIKs>& iks) noexcept;

        void apply_ik(const BoneIndex& index);

        // 膝
        void add_knee(const types::BoneIndex& index);
        // 任意軸(内部で正規化される)
        void add_hinge(const types::BoneIndex& index, const glm::vec4& axis);
    };
} // namespace enishi::animation