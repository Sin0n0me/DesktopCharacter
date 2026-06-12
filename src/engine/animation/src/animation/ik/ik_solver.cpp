#include "ik_solver.h"
#include <algorithm>
#include <cmath>
#include <glm/gtx/perpendicular.hpp>

namespace enishi::animation {
    constexpr float EPSILON = 1e-5f;

    IKSolver::IKSolver(const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<const PMDIKs>& iks) noexcept
        : bone_accessor(bone_accessor)
        , ik_map()
        , hinge_map() {
        for (const auto& ik : iks->iks) {
            const auto ik_bone = ik.ik_bone;
            this->ik_map[ik_bone] = ik;
            this->ik_quaternion_map[ik_bone] = glm::XMQuaternionIdentity();
        }
    }

    void IKSolver::apply_ik(const BoneIndex& bone_index) {
        const auto iter = this->ik_map.find(bone_index);
        if (iter == this->ik_map.end()) {
            return;
        }

        const auto& ik = iter->second;
        if (ik.chain.empty()) {
            return;
        }

        for (const auto& index : ik.chain) {
            const auto bone_node = this->bone_accessor->get_bone_node(index).get();
            bone_node->set_ik_rotate(glm::XMQuaternionIdentity());
            bone_node->update_local();
            bone_node->update_global();
        }

        const auto ik_node = this->bone_accessor->get_bone_node(ik.ik_bone).get();
        const auto target_node = this->bone_accessor->get_bone_node(ik.target_bone).get();
        float max_distance = std::numeric_limits<float>::max();
        for (int i = 0; i < ik.iterations; ++i) {
            for (const auto& index : ik.chain) {
                const auto bone_node = this->bone_accessor->get_bone_node(index).get();
                this->solve_ik_bone(index, bone_node, ik_node, target_node, Radian(ik.limit));
            }

            // 発散防止
            const glm::vec4 ik_pos = ik_node->get_global_position();
            const glm::vec4 target_pos = target_node->get_global_position();
            const float distance = glm::length(target_pos - ik_pos);
            if (distance < max_distance) {
                max_distance = distance;
                for (const auto& index : ik.chain) {
                    const auto bone_node = this->bone_accessor->get_bone_node(index).get();
                    this->ik_quaternion_map[index] = bone_node->get_ik_rotate();
                }
            } else {
                for (const auto& index : ik.chain) {
                    const auto bone_node = this->bone_accessor->get_bone_node(index).get();
                    bone_node->set_ik_rotate(this->ik_quaternion_map.at(index));
                    bone_node->update_local();
                    bone_node->update_global();
                }
                break;
            }
        }
    }

    void IKSolver::add_knee(const types::BoneIndex& index) {
        // X軸固定
        this->hinge_map[index] = glm::normalize(glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
    }

    void IKSolver::add_hinge(const types::BoneIndex& index, const glm::vec4& axis) {
        this->hinge_map[index] = glm::normalize(axis);
    }

    void IKSolver::solve_ik_bone(const BoneIndex& index,
        BoneNode* const bone_node,
        const BoneNode* ik_bone_node,
        const BoneNode* target_bone_node,
        const float& ik_limit) const {
        const MMDMatrix inv_bone = bone_node->get_global().inverse(); // inverse用
        const glm::vec4 bone_pos = bone_node->get_global_position();
        const glm::vec4 ik_pos =
            glm::XMVectorSubtract(ik_bone_node->get_global_position(), bone_pos);
        const glm::vec4 target_pos =
            glm::XMVectorSubtract(target_bone_node->get_global_position(), bone_pos);
        const glm::vec4 ik_vec = glm::normalize(glm::XMVector3TransformNormal(ik_pos, inv_bone));
        const glm::vec4 target_vec =
            glm::normalize(glm::XMVector3TransformNormal(target_pos, inv_bone));

        //
        const float dist = glm::length(target_vec - ik_vec);
        if (dist < EPSILON) {
            return;
        }

        const float dot = std::clamp(glm::dot(target_vec, ik_vec), -1.0f, 1.0f);

        // acos(dot)=[0, π]
        const auto angle = std::min(std::acos(dot), ik_limit);
        if (angle < EPSILON) {
            return;
        }

        // 制限なしCCD IK
        {
            // 回転軸決め
            const glm::vec4 cross = glm::normalize(glm::cross(target_vec, ik_vec));
            if (glm::length(cross) < EPSILON) {
                return;
            }

            // 回転の作成
            const glm::vec4 rotation = angle * cross;

            // ik_rotate = normalize(inv(A) * R * A * IK)
            const auto& animation_rotate = bone_node->get_animation_rotate();
            const auto& inv_animation_rotate = glm::inverse(animation_rotate);
            const auto q =
                inv_animation_rotate * rotation * animation_rotate * bone_node->get_ik_rotate();

            // セット時に正規化される
            bone_node->set_ik_rotate(q);
        }

        // 制限ありCCD IK
        // スイング・ツイスト分解によるクオータニオンを分解してから制限をかけるので
        // CCD IK適用後に行う
        const auto& iter = this->hinge_map.find(index);
        if (iter != this->hinge_map.end()) {
            const glm::vec4 twist_axis = iter->second;

            // 回転の適用
            const auto q = IKSolver::decompose_swing_twist(bone_node->get_ik_rotate(),
                twist_axis,
                0.0f,
                ik_limit,
                0.0f // スイングは禁止
            );
            bone_node->set_ik_rotate(q);
        }

        // bone_node->update_local();
        //  bone_node->update_global();
    }

    glm::quat IKSolver::decompose_swing_twist(const glm::quat& q,
        const glm::vec3& twist_axis,
        const float twist_min,
        const float twist_max,
        const float swing_max) {
        const glm::vec3 decomp_vec = glm::vec3(q.x, q.y, q.z);
        const auto decomp_w = q.w;

        // 射影によるTwistの抽出
        const glm::vec3 projection = twist_axis * glm::dot(decomp_vec, twist_axis);
        const glm::quat twist =
            glm::normalize(glm::quat(projection.x, projection.y, projection.z, decomp_w));

        // Twistの角度の取得
        const glm::vec3 twist_vec = glm::vec3(twist.x, twist.y, twist.z);
        auto twist_angle = 2.0f * std::atan2(glm::length(twist_vec), twist.w);

        // 符号補正(軸方向)
        if (glm::dot(twist_axis, twist_vec) < 0.0f) {
            twist_angle *= -1.0;
        }
        twist_angle = glm::clamp(twist_angle, twist_min, twist_max);

        const glm::quat limited_twist = glm::angleAxis(twist_angle, twist_axis);

        // Swing 成分
        const glm::quat twist_inverse = glm::inverse(twist);
        const glm::quat swing = glm::normalize(q * twist_inverse);

        // 再合成
        return IKSolver::clamp_swing_cone(swing, twist_axis, swing_max) * limited_twist;
    }

    glm::quat IKSolver::clamp_swing_cone(
        const glm::quat& swing, const glm::vec3& twist_axis, const float max) {
        // スイング後の軸方向
        const glm::vec3 d = swing * twist_axis;
        const auto cos_theta = glm::dot(twist_axis, d);
        const auto cos_theta_max = std::cos(max);

        // 制限内ならそのまま返す
        if (cos_theta_max <= cos_theta) {
            return swing;
        }

        // 制限外は円錐表面へ射影

        // 分解
        const glm::vec3 parallel = twist_axis * cos_theta;
        const glm::vec3 perpendicular = d - parallel;
        if (glm::length(perpendicular) < EPSILON) {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        const glm::vec3 v = glm::normalize(perpendicular);

        // 最大角で再構成
        const glm::vec3 clamped = glm::normalize(twist_axis * cos_theta_max + v * std::sin(max));

        // 新しい Swing クォータニオンを構成
        return IKSolver::quaternion_from_to(twist_axis, clamped);
    }

    glm::quat IKSolver::quaternion_from_to(const glm::quat& from, const glm::quat& to) {
        const float dot = glm::dot(from, to);

        // ほぼ同一方向
        if (dot > 0.999999f) {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        // ほぼ反対方向
        if (dot < -0.999999f) {
            constexpr auto PI = glm::pi<float>();

            if (std::fabs(from.z) < std::fabs(from.x)) {
                // Z成分の絶対値がX成分より小さい場合
                // (-v.y, v.x, 0.0f) に相当するベクトルを返す
                return glm::vec3(-from.y, from.x, 0.0f);
            } else {
                // (0.0f, -v.z, v.y) に相当するベクトルを返す
                return glm::vec3(0.0f, -from.z, from.y);
            }

            glm::ortho(from);
            return glm::angleAxis(glm::normalize(glm::XMVector3Orthogonal(from)), PI);
        }

        const glm::quat axis = glm::normalize(glm::cross(from, to));
        return glm::angleAxis(std::acos(dot), glm::vec3(axis.x, axis.y, axis.z));
    }
} // namespace enishi::animation