#include "../../../../math/Angle.h"
#include "../bone/BoneNode.h"
#include "../bone/IBoneAccessor.h"
#include "../PMDFileStruct.h"
#include "IKSolver.h"
#include <algorithm>
#include <cmath>
#include <DirectXMath.h>

constexpr float EPSILON = 1e-5f;

IKSolver::IKSolver(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<const PMDIKs>& iks
) noexcept :
    bone_accessor(bone_accessor),
    ik_map(),
    hinge_set() {
    for(const auto& ik : iks->iks) {
        this->ik_map[ik.ik_bone] = ik;
    }

    // TODO: 削除
    this->hinge_set.insert(5);
    this->hinge_set.insert(8);
}

void IKSolver::apply_ik(const BoneIndex& bone_index) const {
    const auto iter = this->ik_map.find(bone_index);
    if(iter == this->ik_map.end()) {
        return;
    }

    const auto& ik = iter->second;
    if(ik.chain.empty()) {
        return;
    }

    const auto ik_node = this->bone_accessor->get_bone_node(ik.ik_bone).get();
    const auto target_node = this->bone_accessor->get_bone_node(ik.target_bone).get();
    for(int i = 0; i < ik.iterations; ++i) {
        for(const auto& index : ik.chain) {
            const auto bone_node = this->bone_accessor->get_bone_node(index).get();
            this->solve_ik_bone(
                bone_node,
                ik_node,
                target_node,
                Radian(ik.limit),
                this->hinge_set.contains(index)
            );
        }
    }
}

void IKSolver::solve_ik_bone(
    BoneNode* const bone_node,
    const BoneNode* ik_bone_node,
    const BoneNode* target_bone_node,
    const Radian<float>& ik_limit,
    const bool use_hinge
) const {
    const DirectX::XMVECTOR bone_pos = bone_node->get_global_position(); // inverse用
    const DirectX::XMVECTOR ik_pos = ik_bone_node->get_global_position();
    const DirectX::XMVECTOR target_pos = target_bone_node->get_global_position();

    const DirectX::XMVECTOR ik_vec = DirectX::XMVector3Normalize(
        DirectX::XMVectorSubtract(ik_pos, bone_pos)
    );
    const DirectX::XMVECTOR target_vec = DirectX::XMVector3Normalize(
        DirectX::XMVectorSubtract(target_pos, bone_pos)
    );
    const float dist = DirectX::XMVectorGetX(
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(target_pos, ik_pos)
        )
    );
    if(dist < EPSILON) {
        return;
    }

    const float dot = std::clamp(
        DirectX::XMVectorGetX(
            DirectX::XMVector3Dot(target_vec, ik_vec)
        ),
        -1.0f,
        1.0f
    );

    const auto angle = Radian(std::min(std::acos(dot), ik_limit.get()));
    if(angle < Radian(EPSILON)) {
        return;
    }

    // 制限なしCCD IK
    {
        // 回転軸
        const DirectX::XMVECTOR cross = DirectX::XMVector3Normalize(
            DirectX::XMVector3Cross(target_vec, ik_vec)
        );
        if(DirectX::XMVector3Equal(cross, DirectX::XMVectorZero())) {
            return;
        }

        const DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(
            cross,
            angle.get()
        );

        bone_node->set_rotate(
            DirectX::XMQuaternionMultiply(
                quaternion,
                bone_node->get_rotate()
            )
        );
    }

    // 制限ありCCD IK(通常のCCD IK適用後に行う)
    // スイング・ツイスト分解によるクオータニオンを分解し制限をかける
    if(use_hinge) {
        // TODO: 任意の軸制限
        const DirectX::XMVECTOR twist_axis = DirectX::XMVector3Normalize(
            DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f) // X軸固定
        );

        // 回転の適用
        bone_node->set_rotate(
            IKSolver::decompose_swing_twist(
                bone_node->get_rotate(),
                twist_axis,
                Radian(0.0f),
                ik_limit,
                Radian(0.0f) // スイングは禁止
            )
        );
    }

    bone_node->update_local();
    bone_node->update_global();
}

DirectX::XMVECTOR IKSolver::decompose_swing_twist(
    const DirectX::XMVECTOR& q,
    const DirectX::XMVECTOR& twist_axis,
    const Radian<float>& twist_min,
    const Radian<float>& twist_max,
    const Radian<float>& swing_max
) {
    const DirectX::XMVECTOR decomp_vec = DirectX::XMVectorSetW(q, 0.0f);
    const auto decomp_w = DirectX::XMVectorGetW(q);

    // 射影によるTwistの抽出
    const DirectX::XMVECTOR projection = DirectX::XMVectorScale(
        twist_axis,
        DirectX::XMVectorGetX(
            DirectX::XMVector3Dot(decomp_vec, twist_axis)
        )
    );
    const DirectX::XMVECTOR twist = DirectX::XMQuaternionNormalize(
        DirectX::XMVectorSet(
            DirectX::XMVectorGetX(projection),
            DirectX::XMVectorGetY(projection),
            DirectX::XMVectorGetZ(projection),
            decomp_w
        )
    );

    // Twistの角度の取得
    const DirectX::XMVECTOR twist_vec = DirectX::XMVectorSetW(twist, 0.0f);
    auto twist_angle = Radian(
        2.0f * std::atan2(
            DirectX::XMVectorGetX(DirectX::XMVector3Length(twist_vec)),
            DirectX::XMVectorGetW(twist)
        )
    );

    // 符号補正(軸方向)
    if(DirectX::XMVectorGetX(DirectX::XMVector3Dot(twist_axis, twist_vec)) < 0.0f) {
        twist_angle *= -1.0;
    }
    twist_angle.clamp(twist_min, twist_max);

    // 事前にAxisが正規化済みであればXMQuaternionRotationAxisより
    // XMQuaternionRotationNormalのほうが高速
    //
    // > If Axis is a normalized vector, it is faster to use XMQuaternionRotationNormal
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmquaternionrotationaxis
    const DirectX::XMVECTOR limited_twist = DirectX::XMQuaternionRotationNormal(
        twist_axis,
        twist_angle.get()
    );

    // Swing 成分
    const DirectX::XMVECTOR twist_inverse = DirectX::XMQuaternionInverse(twist);
    const DirectX::XMVECTOR swing = DirectX::XMQuaternionNormalize(
        DirectX::XMQuaternionMultiply(
            q,
            twist_inverse
        )
    );

    // 再合成
    return DirectX::XMQuaternionMultiply(
        IKSolver::clamp_swing_cone(swing, twist_axis, swing_max),
        limited_twist
    );
}

DirectX::XMVECTOR IKSolver::clamp_swing_cone(
    const DirectX::XMVECTOR& swing,
    const DirectX::XMVECTOR& twist_axis,
    const Radian<float>& max
) {
    // スイング後の軸方向
    const DirectX::XMVECTOR d = DirectX::XMVector3Rotate(
        twist_axis,
        swing
    );

    const auto cos_theta = Radian(
        DirectX::XMVectorGetX(
            DirectX::XMVector3Dot(twist_axis, d)
        )
    );
    const auto cos_theta_max = max.cos();

    // 制限内ならそのまま返す
    if(cos_theta_max <= cos_theta) {
        return swing;
    }

    // 制限外は円錐表面へ射影

    // 分解
    const DirectX::XMVECTOR parallel = DirectX::XMVectorScale(
        twist_axis,
        cos_theta.get()
    );

    const DirectX::XMVECTOR perpendicular = DirectX::XMVectorSubtract(d, parallel);
    if(DirectX::XMVector3LengthSq(perpendicular).m128_f32[0] < EPSILON) {
        return DirectX::XMQuaternionIdentity();
    }

    const DirectX::XMVECTOR v = DirectX::XMVector3Normalize(perpendicular);

    // 最大角で再構成
    const DirectX::XMVECTOR clamped = DirectX::XMVector3Normalize(
        DirectX::XMVectorAdd(
            DirectX::XMVectorScale(twist_axis, cos_theta_max.get()),
            DirectX::XMVectorScale(v, max.sin().get())
        )
    );

    // 新しい Swing クォータニオンを構成
    return IKSolver::quaternion_from_to(twist_axis, clamped);
}

DirectX::XMVECTOR IKSolver::quaternion_from_to(const DirectX::XMVECTOR& from, const DirectX::XMVECTOR& to) {
    const float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(from, to));

    // ほぼ同一方向
    if(dot > 0.999999f) {
        return DirectX::XMQuaternionIdentity();
    }

    // ほぼ反対方向
    if(dot < -0.999999f) {
        return DirectX::XMQuaternionRotationNormal(
            DirectX::XMVector3Normalize(
                DirectX::XMVector3Orthogonal(from)
            ),
            DirectX::XM_PI
        );
    }

    return DirectX::XMQuaternionRotationNormal(
        DirectX::XMVector3Normalize(
            DirectX::XMVector3Cross(from, to)
        ),
        std::acos(dot)
    );
}