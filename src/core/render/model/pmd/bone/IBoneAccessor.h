#pragma once

#include "../bone/Bone.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class BoneNode;
class IKSolver;

class IBoneAccessor {
public:
    virtual ~IBoneAccessor(void) noexcept = default;

    virtual std::shared_ptr<BoneNode> get_bone_node(const BoneIndex& index) const = 0;

    virtual std::optional<BoneIndex> get_bone_index(const std::string& name) const = 0;

    virtual std::vector<std::shared_ptr<BoneNode>> get_root_bones(void) const = 0;
};
