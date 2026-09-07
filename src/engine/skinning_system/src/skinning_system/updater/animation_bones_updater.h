#pragma once
#include <platform/skinning_system/interface_bone_updater.h>
#include <skinning_system/views/animation_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    class AnimationBonesUpdater : public platform::IBoneUpdater {
      private:
        std::span<const types::BoneNode> bone_nodes;
        std::vector<platform::IAnimationBoneView*>* const animation_view;

      public:
        void update_local(const types::BoneIndex index) noexcept override;
        void update_global(const types::BoneIndex index) noexcept override;
        void update_children_global(const types::BoneIndex index) noexcept override;
    };
} // namespace enishi::skinning_system