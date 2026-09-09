#pragma once
#include <platform/physics/bone/interface_physics_bone_view_list.h>
#include <skinning_system/views/physics_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    class PhysicsBonesCache : public platform::IPhysicsBoneViewList {
      private:
        std::span<const types::BoneNode> bone_nodes;
        std::vector<PhysicsBoneView> physics_view;

      public:
        void rebuild();

        std::size_t size(void) const noexcept override;
        foundation::Option<platform::IPhysicsBoneView*> get(
            const types::BoneIndex index) noexcept override;
        foundation::Option<const platform::IPhysicsBoneView*> get(
            const types::BoneIndex index) const noexcept override;
        platform::IPhysicsBoneView* at(const types::BoneIndex index) noexcept override;
        const platform::IPhysicsBoneView* at(const types::BoneIndex index) const noexcept override;
    };
} // namespace enishi::skinning_system