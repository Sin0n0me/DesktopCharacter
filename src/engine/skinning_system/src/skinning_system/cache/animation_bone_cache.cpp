#include "animation_bone_cache.h"

namespace enishi::skinning_system {
    platform::IAnimationBoneView* AnimationBonesCache::at(const types::BoneIndex index) noexcept {
        return &this->animation_views[index];
    }
    const platform::IAnimationBoneView* AnimationBonesCache::at(
        const types::BoneIndex index) const noexcept {
        return &this->animation_views[index];
    }
    foundation::Option<platform::IAnimationBoneView*> AnimationBonesCache::get(
        const types::BoneIndex index) noexcept {
        if (this->animation_views.size() < index + 1) {
            return {};
        }
        return &this->animation_views[index];
    }
    foundation::Option<const platform::IAnimationBoneView*> AnimationBonesCache::get(
        const types::BoneIndex index) const noexcept {
        if (this->animation_views.size() < index + 1) {
            return {};
        }
        return &this->animation_views[index];
    }
} // namespace enishi::skinning_system