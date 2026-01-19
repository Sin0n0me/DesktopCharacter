#pragma once

#include "../bone/Bone.h"
#include "BoneNode.h"
#include <memory>
#include <string>

struct Bones;
class IKSolver;

class IBoneAccessor {
public:
    virtual ~IBoneAccessor(void) noexcept = default;

    virtual std::shared_ptr<Bones> get_mutable_bones(void) const = 0;

    virtual std::shared_ptr<std::vector<BoneNode>> get_mutable_bone_nodes(void) const = 0;

    virtual const PMDBoneData& get_bone(const BoneIndex& index) const = 0;

    virtual std::shared_ptr<const IKSolver> get_ik_soulver(void) const = 0;

    virtual BoneIndex get_bone_index(const std::string& name) const = 0;

    virtual const std::unordered_map<std::string, BoneIndex>& get_bone_name_map(void) const = 0;
};
