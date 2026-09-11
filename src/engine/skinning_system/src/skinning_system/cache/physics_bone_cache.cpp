#include "physics_bone_cache.h"

namespace enishi::skinning_system {
    PhysicsBonesCache::PhysicsBonesCache(std::span<const types::BoneNode> bone_nodes,
        std::vector<std::unique_ptr<PhysicsBoneView>>&& physics_views)
        : bone_nodes(bone_nodes)
        , physics_views(std::move(physics_views)) {
    }

    std::span<const types::BoneNode> PhysicsBonesCache::get_bone_nodes(void) const noexcept {
        return this->bone_nodes;
    }

    std::size_t PhysicsBonesCache::size(void) const noexcept {
        return this->physics_views.size();
    }

    foundation::Option<platform::IPhysicsBoneView*> PhysicsBonesCache::get(
        const types::BoneIndex index) noexcept {
        if (this->physics_views.size() <= index) {
            return {};
        }
        return this->physics_views[index].get();
    }
    foundation::Option<const platform::IPhysicsBoneView*> PhysicsBonesCache::get(
        const types::BoneIndex index) const noexcept {
        if (this->physics_views.size() <= index) {
            return {};
        }
        return this->physics_views[index].get();
    }
    platform::IPhysicsBoneView* PhysicsBonesCache::at(const types::BoneIndex index) noexcept {
        return this->physics_views[index].get();
    }
    const platform::IPhysicsBoneView* PhysicsBonesCache::at(
        const types::BoneIndex index) const noexcept {
        return this->physics_views[index].get();
    }
} // namespace enishi::skinning_system