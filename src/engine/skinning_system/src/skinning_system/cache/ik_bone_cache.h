#pragma once
#include <foundation/option/option.h>
#include <memory>
#include <platform/ik/interface_ik_bone_view_list.h>
#include <skinning_system/views/ik_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    /**
     * モデル1体分のIKBoneViewをまとめて保持し、platform::IIKBoneViewListとして公開するキャッシュ
     * AnimationBonesCacheと同じ設計方針
     * resize時はこのキャッシュ自体を再作成する
     */
    class IKBoneCache final : public platform::IIKBoneViewList {
      private:
        std::span<const types::BoneNode> bone_nodes;
        std::vector<std::unique_ptr<IKBoneView>> ik_views;

      public:
        IKBoneCache(std::span<const types::BoneNode> bone_nodes,
            std::vector<std::unique_ptr<IKBoneView>>&& ik_views);

        [[nodiscard]] std::span<const types::BoneNode> get_bone_nodes(void) const noexcept;

        std::size_t size(void) const noexcept override;
        foundation::Option<platform::IIKBoneView*> get(
            const types::BoneIndex index) noexcept override;
        foundation::Option<const platform::IIKBoneView*> get(
            const types::BoneIndex index) const noexcept override;
        platform::IIKBoneView* at(const types::BoneIndex index) noexcept override;
        const platform::IIKBoneView* at(const types::BoneIndex index) const noexcept override;
    };
} // namespace enishi::skinning_system
