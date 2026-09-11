#pragma once
#include <memory>
#include <platform/asset/bone/interface_bind_bone_view_list.h>
#include <skinning_system/views/bind_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    /**
     * モデル1体分のBindBoneViewをまとめて保持し
     * platform::IBindBoneViewListとして公開するキャッシュ
     * バインドポーズはモデル読み込み後に変化しないため読み取り専用として扱う
     * resize時はこのキャッシュ自体を再作成する
     */
    class BindBonesCache final : public platform::IBindBoneViewList {
      private:
        std::vector<std::unique_ptr<BindBoneView>> bind_views;

      public:
        explicit BindBonesCache(std::vector<std::unique_ptr<BindBoneView>>&& bind_views);

        std::size_t size(void) const noexcept override;
        const platform::IBindBoneView* at(const types::BoneIndex index) const noexcept override;
    };
} // namespace enishi::skinning_system
