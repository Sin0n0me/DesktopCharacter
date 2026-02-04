#pragma once
#include "../../pmd/PMDFileStruct.h"
#include "../bone/Bone.h"
#include "../bone/BoneNode.h"
#include <cstdint>
#include <memory>
#include <set>
#include <vector>

class IKSolver {
private:
    const std::shared_ptr<const PMDBoneMap> bone_map;
    std::vector<std::vector<BoneIndex>> childern_tree;
    std::unordered_map<BoneIndex, PMDIK> ik_map; // first: index, second: ik
    std::set<BoneIndex> hinge_set; //膝などヒンジ関節である場合

    void update_children_global(
        const uint16_t& root,
        std::vector<BoneNode>& bone_matricies
    ) const;

    void solve_ik_bone(
        const PMDIK& ik,
        const uint16_t& bone_index,
        std::vector<BoneNode>& bone_matricies,
        const int iteration
    ) const;

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

public:

    explicit IKSolver(
        const std::shared_ptr<const PMDIKs>& iks,
        const std::shared_ptr<const PMDBoneMap>& bone_map
    ) noexcept;

    void apply_ik(
        const BoneIndex& bone_index,
        std::vector<BoneNode>& bone_matricies
    ) const;
};
