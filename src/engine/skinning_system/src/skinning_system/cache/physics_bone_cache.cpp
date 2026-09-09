#include "physics_bone_cache.h"

namespace enishi::skinning_system {
    std::size_t PhysicsBonesCache::size(void) const noexcept {
        return this->physics_view.size();
    }
    foundation::Option<platform::IPhysicsBoneView*> PhysicsBonesCache::get(
        const types::BoneIndex index) noexcept {
        if (this->physics_view.size() < index + 1) {
            return {};
        }
        return &this->physics_view[index];
    }
    foundation::Option<const platform::IPhysicsBoneView*> PhysicsBonesCache::get(
        const types::BoneIndex index) const noexcept {
        if (this->physics_view.size() < index + 1) {
            return {};
        }
        return &this->physics_view[index];
    }
    platform::IPhysicsBoneView* PhysicsBonesCache::at(const types::BoneIndex index) noexcept {
        return &this->physics_view[index];
    }
    const platform::IPhysicsBoneView* PhysicsBonesCache::at(
        const types::BoneIndex index) const noexcept {
        return &this->physics_view[index];
    }
} // namespace enishi::skinning_system