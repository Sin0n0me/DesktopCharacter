#pragma once

#include "../bone/Bone.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class BoneNode;
class IKSolver;

using BoneNodePtr = std::shared_ptr<BoneNode>;

class IBoneAccessor {
public:
    virtual ~IBoneAccessor(void) noexcept = default;

    virtual const std::vector<BoneNodePtr>& get_all_bone_nodes(void) const = 0;

    virtual BoneNodePtr get_bone_node(const BoneIndex& index) const = 0;

    virtual std::optional<BoneIndex> get_bone_index(
        const std::string& name
    ) const = 0;

    virtual std::vector<BoneNodePtr> get_root_bones(void) const = 0;
};
