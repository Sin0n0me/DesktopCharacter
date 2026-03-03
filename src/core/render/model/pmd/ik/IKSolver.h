#pragma once
#include "../../../../math/Angle.h"
#include "../../../../math/WrappedMatrix.h"
#include "../../pmd/PMDFileStruct.h"
#include "../bone/Bone.h"
#include <memory>
#include <set>
#include <unordered_map>

class BoneNode;
class IBoneAccessor;

class IKSolver {
private:
    const std::shared_ptr<const IBoneAccessor> bone_accessor;
    std::unordered_map<BoneIndex, PMDIK> ik_map; // first: index, second: ik
    std::unordered_map<BoneIndex, Vector4> ik_quaternion_map; // first: index, second: ik(quaternion)
    std::unordered_map<BoneIndex, Vector4> hinge_map; // first: index, second: ヒンジ軸 膝などヒンジ関節用

protected:
    static Vector4 decompose_swing_twist(
        const Vector4& q,
        const Vector4& twist_axis,
        const Radian<float>& twist_min,
        const Radian<float>& twist_max,
        const Radian<float>& swing_max
    );

    static Vector4 clamp_swing_cone(
        const Vector4& swing,
        const Vector4& twist_axis,
        const Radian<float>& max
    );

    static Vector4 quaternion_from_to(
        const Vector4& from,
        const Vector4& to
    );

protected:
    void solve_ik_bone(
        const BoneIndex& index,
        BoneNode* const bone_node,
        const BoneNode* ik_bone_node,
        const BoneNode* target_bone_node,
        const Radian<float>& ik_limit
    ) const;

public:

    explicit IKSolver(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<const PMDIKs>& iks
    ) noexcept;

    void apply_ik(const BoneIndex& index);

    // 膝
    void add_knee(const BoneIndex& index);
    // 任意軸(内部で正規化される)
    void add_hinge(const BoneIndex& index, const Vector4& axis);
};
