#pragma once
#include <platform/skinning_system/interface_bone_updater.h>
#include <skinning_system/cache/physics_bone_cache.h>
#include <skinning_system/views/physics_bone_view.h>
#include <span>
#include <vector>

namespace enishi::skinning_system {
    class PhysicsBonesUpdater : public platform::IBoneUpdater {
      private:
        std::span<const types::BoneNode> bone_nodes;
        PhysicsBonesCache* const physics_view;

      public:
        void update_local(const types::BoneIndex index) noexcept override;
        void update_global(const types::BoneIndex index) noexcept override;
        void update_children_global(const types::BoneIndex index) noexcept override;
        void update_global_form_roots(void) noexcept override;
    };
} // namespace enishi::skinning_system