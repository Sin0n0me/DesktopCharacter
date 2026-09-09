#include "animation_bones_updater.h"

namespace enishi::skinning_system {
    void AnimationBonesUpdater::update_local(const types::BoneIndex index) noexcept {
    }

    void AnimationBonesUpdater::update_global(const types::BoneIndex index) noexcept {
        if (this->animation_view->size() < index + 1) {
            return;
        }
        if (this->bone_nodes.size() < index + 1) {
            return;
        }
        const auto& bone_node = this->bone_nodes[index];
        const auto& view = this->animation_view->at(index);

        const auto& local = view->get_animation_local_transform();
        if (bone_node.has_parent()) {
            // global = parent_global * local
            const auto& prent_node = this->animation_view->at(bone_node.parent);
            const auto& prent_global = prent_node->get_animation_global_transform();
            view->set_animation_global_transform(prent_global * local);
        } else {
            // global = local
            view->set_animation_global_transform(local);
        }

        this->update_children_global(index);
    }

    void AnimationBonesUpdater::update_children_global(const types::BoneIndex index) noexcept {
        if (this->bone_nodes.size() < index + 1) {
            return;
        }
        const auto& bone_node = this->bone_nodes[index];
        for (const auto& child : bone_node.children) {
            this->update_global(child);
        }
    }
    void skinning_system::AnimationBonesUpdater::update_global_form_roots(void) noexcept {
    }
} // namespace enishi::skinning_system