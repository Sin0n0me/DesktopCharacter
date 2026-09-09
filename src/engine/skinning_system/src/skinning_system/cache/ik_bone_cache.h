#pragma once
#include <platform/ik/interface_ik_bone_view_list.h>
#include <skinning_system/views/ik_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    class IKBoneCache : public platform::IIKBoneViewList {
      private:
        std::span<const types::BoneNode> bone_nodes;
        std::vector<IKBoneView> physics_view;

      public:
        void rebuild();

        std::size_t size(void) const noexcept override;
        foundation::Option<platform::IIKBoneView*> get(
            const types::BoneIndex index) noexcept override;
        foundation::Option<const platform::IIKBoneView*> get(
            const types::BoneIndex index) const noexcept override;
        platform::IIKBoneView* at(const types::BoneIndex index) noexcept override;
        const platform::IIKBoneView* at(const types::BoneIndex index) const noexcept override;
    };
} // namespace enishi::skinning_system