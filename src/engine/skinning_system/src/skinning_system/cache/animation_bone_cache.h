#pragma once
#include <foundation/option/option.h>
#include <memory>
#include <platform/animation/interface_animation_bone_view_list.h>
#include <skinning_system/views/animation_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    /**
     * モデル1体分のAnimationBoneViewをまとめて保持し、
     * platform::IAnimationBoneViewListとして公開するキャッシュ
     * resize時はこのキャッシュ自体を再作成する
     */
    class AnimationBonesCache final : public platform::IAnimationBoneViewList {
      private:
        std::span<const types::BoneNode> bone_nodes;
        std::vector<std::unique_ptr<AnimationBoneView>> animation_views;

      public:
        explicit AnimationBonesCache(std::span<const types::BoneNode> bone_nodes,
            std::vector<std::unique_ptr<AnimationBoneView>>&& animation_views);

        [[nodiscard]] std::span<const types::BoneNode> get_bone_nodes(void) const noexcept;

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
