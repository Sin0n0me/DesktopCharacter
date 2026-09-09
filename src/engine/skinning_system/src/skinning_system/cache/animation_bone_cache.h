#pragma once
#include <platform/animation/interface_animation_bone_view_list.h>
#include <skinning_system/views/animation_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    class AnimationBonesCache : public platform::IAnimationBoneViewList {
      private:
        std::span<const types::BoneNode> bone_nodes;
        std::vector<AnimationBoneView> animation_views;

      public:
        void rebuild();

        std::size_t size(void) const noexcept override;
        platform::IAnimationBoneView* at(const types::BoneIndex index) noexcept override;
        const platform::IAnimationBoneView* at(
            const types::BoneIndex index) const noexcept override;
        foundation::Option<platform::IAnimationBoneView*> get(
            const types::BoneIndex index) noexcept override;
        foundation::Option<const platform::IAnimationBoneView*> get(
            const types::BoneIndex index) const noexcept override;
    };
} // namespace enishi::skinning_system