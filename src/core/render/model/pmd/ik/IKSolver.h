#pragma once
#include "../../pmd/PMDFileStruct.h"
#include "../bone/Bone.h"
#include <DirectXMath.h>
#include <memory>
#include <set>
#include <unordered_map>

class BoneNode;
class IBoneAccessor;

class IKSolver {
private:
    const std::shared_ptr<const IBoneAccessor> bone_accessor;
    std::unordered_map<BoneIndex, PMDIK> ik_map; // first: index, second: ik
    std::set<BoneIndex> hinge_set; //膝などヒンジ関節である場合

protected:
    static DirectX::XMVECTOR decompose_swing_twist(
        const DirectX::XMVECTOR& q,
        const DirectX::XMVECTOR& twist_axis,
        const float twist_min,
        const float twist_max,
        const float swing_max
    );

    static DirectX::XMVECTOR clamp_swing_cone(
        const DirectX::XMVECTOR& swing,
        const DirectX::XMVECTOR& twist_axis,
        const float max
    );

    static DirectX::XMVECTOR quaternion_from_to(
        const DirectX::XMVECTOR& from,
        const DirectX::XMVECTOR& to
    );

protected:
    void solve_ik_bone(
        BoneNode* const bone_node,
        const BoneNode* ik_bone_node,
        const BoneNode* target_bone_node,
        const float ik_limit,
        const bool use_hinge
    ) const;

public:

    explicit IKSolver(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<const PMDIKs>& iks
    ) noexcept;

    void apply_ik(const BoneIndex& index) const;
};
