#include "../../../../utility/Utility.h"
#include "..//PMDFileStruct.h"
#include "IKSolver.h"
#include <algorithm>
#include <cmath>
#include <DirectXMath.h>
#include <stack>

constexpr float EPSILON = 1e-5f;

IKSolver::IKSolver(
    const std::shared_ptr<const PMDIKs>& iks,
    const std::shared_ptr<const PMDBoneMap>& bone_map
) noexcept : bone_map(bone_map) {
    for(const auto& ik : iks->iks) {
        this->ik_map[ik.ik_bone] = ik;
    }

    this->childern_tree.resize(this->bone_map->size());
    for(const auto& [index, data] : *this->bone_map) {
        if(data.parent < 0) {
            continue;
        }

        this->childern_tree[data.parent].emplace_back(index);
    }

    for(auto& iter : this->childern_tree) {
        std::sort(iter.begin(), iter.end(), std::greater<>());
    }

    // TODO: 削除
    this->hinge_set.insert(5);
    this->hinge_set.insert(8);
}

void IKSolver::apply_ik(
    const BoneIndex& bone_index,
    std::vector<BoneNode>& bone_matricies
) const {
    const auto& ik = this->ik_map.at(bone_index);
    if(ik.chain.empty()) {
        return;
    }

    for(int i = 0; i < ik.iterations; ++i) {
        for(const auto& bone_index : ik.chain) {
            this->solve_ik_bone(ik, bone_index, bone_matricies, i);
        }
    }
}

// 再帰でもよかったかもしれない
void IKSolver::update_children_global(
    const uint16_t& root,
    std::vector<BoneNode>& bone_matricies
) const {
    // 初期ヒープサイズによっては毎回メモリ確保が起こり重くなるので修正が必要かもしれない
    std::stack<BoneIndex> stack = std::stack<BoneIndex>();
    stack.push(root);

    while(!stack.empty()) {
        const auto bone_index = stack.top();
        stack.pop();

        const auto parent = this->bone_map->at(bone_index).parent;
        auto& bone_matrix = bone_matricies.at(bone_index);
        if(parent < 0) {
            bone_matrix.global = bone_matrix.local;
        } else {
            const auto& parent_global = bone_matricies.at(parent).global;
            bone_matrix.global = bone_matrix.local * parent_global;
        }

        for(const auto& child : this->childern_tree.at(bone_index)) {
            stack.push(child);
        }
    }
}

void IKSolver::solve_ik_bone(
    const PMDIK& ik,
    const uint16_t& bone_index,
    std::vector<BoneNode>& bone_matricies,
    const int iteration
) const {
    const DirectX::XMVECTOR bone_pos = bone_matricies.at(bone_index).global.r[3]; // invers用
    const DirectX::XMVECTOR ik_pos = bone_matricies.at(ik.ik_bone).global.r[3];
    const DirectX::XMVECTOR target_pos = bone_matricies.at(ik.target_bone).global.r[3];

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

    const float angle_radian = std::min(std::acos(dot), ik.limit);
    if(angle_radian < EPSILON) {
        return;
    }

    //
    auto& matrix = bone_matricies.at(bone_index);

    // 制限なしCCD IK
    // 回転軸
    const DirectX::XMVECTOR cross = DirectX::XMVector3Normalize(
        DirectX::XMVector3Cross(target_vec, ik_vec)
    );
    if(DirectX::XMVector3Equal(cross, DirectX::XMVectorZero())) {
        return;
    }

    const DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationNormal(
        cross,
        angle_radian
    );

    matrix.rotate = DirectX::XMQuaternionNormalize(
        DirectX::XMQuaternionMultiply(
            quaternion,
            matrix.rotate
        )
    );

    if(this->hinge_set.contains(bone_index)) {
        // 制限ありCCD IK
        // スイング・ツイスト分解によるクオータニオンを分解し
        // 制限をかける

        // TODO: 任意の軸制限
        const DirectX::XMVECTOR twist_axis = DirectX::XMVector3Normalize(
            DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f) // X軸固定
        );

        //
        matrix.rotate = IKSolver::decompose_swing_twist(
            matrix.rotate,
            twist_axis,
            0,
            ik.limit,
            0 // スイングは禁止
        );
    }

    const auto& rotate = DirectX::XMMatrixRotationQuaternion(matrix.rotate);
    const auto& bind_local = this->bone_map->at(bone_index).local;
    matrix.local = rotate * bind_local;

    this->update_children_global(bone_index, bone_matricies);
}

DirectX::XMVECTOR IKSolver::decompose_swing_twist(
    const DirectX::XMVECTOR& q,
    const DirectX::XMVECTOR& twist_axis,
    const float twist_min,
    const float twist_max,
    const float swing_max
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
        ));

    // Twistの角度の取得
    const DirectX::XMVECTOR twist_vec = DirectX::XMVectorSetW(twist, 0.0f);
    float twist_angle = 2.0f * std::atan2(
        DirectX::XMVectorGetX(DirectX::XMVector3Length(twist_vec)),
        DirectX::XMVectorGetW(twist)
    );

    // 符号補正(軸方向)
    if(DirectX::XMVectorGetX(DirectX::XMVector3Dot(twist_axis, twist_vec)) < 0.0f) {
        twist_angle *= -1.0;
    }
    const float limited_angle = std::clamp(twist_angle, twist_min, twist_max);

    // 事前にAxisが正規化済みであればXMQuaternionRotationAxisより
    // XMQuaternionRotationNormalのほうが高速
    //
    // > If Axis is a normalized vector, it is faster to use XMQuaternionRotationNormal
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmquaternionrotationaxis
    const DirectX::XMVECTOR limited_twist = DirectX::XMQuaternionRotationNormal(
        twist_axis,
        limited_angle
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
    return DirectX::XMQuaternionNormalize(
        DirectX::XMQuaternionMultiply(
            IKSolver::clamp_swing_cone(swing, twist_axis, swing_max),
            limited_twist
        )
    );
}

DirectX::XMVECTOR IKSolver::clamp_swing_cone(
    const DirectX::XMVECTOR& swing,
    const DirectX::XMVECTOR& twist_axis,
    const float max
) {
    // スイング後の軸方向
    const DirectX::XMVECTOR d = DirectX::XMVector3Rotate(
        twist_axis,
        swing
    );

    const float cos_theta = DirectX::XMVectorGetX(
        DirectX::XMVector3Dot(twist_axis, d)
    );
    const float cos_theta_max = cosf(max);

    // 制限内ならそのまま返す
    if(cos_theta_max <= cos_theta) {
        return swing;
    }

    // 制限外は円錐表面へ射影

    // d を d0 に分解
    const DirectX::XMVECTOR parallel = DirectX::XMVectorScale(
        twist_axis,
        cos_theta
    );

    const DirectX::XMVECTOR perpendicular = DirectX::XMVectorSubtract(d, parallel);
    if(DirectX::XMVector3LengthSq(perpendicular).m128_f32[0] < EPSILON) {
        return DirectX::XMQuaternionIdentity();
    }

    const DirectX::XMVECTOR v = DirectX::XMVector3Normalize(perpendicular);

    // 最大角で再構成
    const DirectX::XMVECTOR clamped = DirectX::XMVector3Normalize(
        DirectX::XMVectorAdd(
            DirectX::XMVectorScale(twist_axis, cos_theta_max),
            DirectX::XMVectorScale(v, std::sin(max))
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
        return DirectX::XMQuaternionRotationAxis(
            DirectX::XMVector3Normalize(
                DirectX::XMVector3Orthogonal(from)
            ),
            DirectX::XM_PI
        );
    }

    const float angle = acosf(dot);
    return DirectX::XMQuaternionRotationAxis(
        DirectX::XMVector3Normalize(
            DirectX::XMVector3Cross(from, to)
        ),
        angle
    );
}