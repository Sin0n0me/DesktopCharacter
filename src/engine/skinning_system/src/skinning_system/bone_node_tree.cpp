#include "bone_node_tree.h"
#include "bone_node.h"

namespace enishi::skinning_system {
    std::shared_ptr<BoneNode> BoneNodeTree::get_node(const types::BoneIndex& index) const {
        return std::shared_ptr<BoneNode>();
    }

    BoneNodeTree::BoneNodes BoneNodeTree::get_root_nodes(const BoneNodes& nodes) {
        BoneNodes root_nodes;

        for (const auto& node : nodes) {
            if (!bool(node)) {
                continue;
            }
            const auto& bone_node = node->get_bone_node();
            if (bone_node.has_parent()) {
                continue;
            }
            root_nodes.emplace_back(node);
        }

        return root_nodes;
    }
} // namespace enishi::skinning_system