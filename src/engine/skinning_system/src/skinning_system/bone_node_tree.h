#pragma once
#include <engine_types/assets/model/addons/bone.h>
#include <memory>
#include <vector>

namespace enishi::platform_impl {
    class BoneNode;
    class BoneNodeTree {
        using BoneNodes = std::vector<std::shared_ptr<BoneNode>>;

      private:
        BoneNodes bone_nodes;
        BoneNodes root_nodes;

      public:
        std::shared_ptr<BoneNode> get_node(const types::BoneIndex& index) const;

      private:
        static BoneNodes get_root_nodes(const BoneNodes& nodes);
    };
} // namespace enishi::platform_impl