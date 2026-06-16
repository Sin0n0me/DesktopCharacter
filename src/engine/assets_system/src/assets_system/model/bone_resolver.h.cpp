#include "bone_resolver.h.h"

namespace enishi::assets_system {
    foundation::Option<types::BoneIndex> BoneResolver::resolve_index(
        const foundation::UTF8& bone_name) const noexcept {
        return this->bone_data->find_bone_index(bone_name);
    }

    foundation::Option<foundation::UTF8> BoneResolver::resolve_name(
        const types::BoneIndex bone_index) const noexcept {
        if (bone_index >= this->bone_data->bones.size()) {
            return {};
        }
        return this->bone_data->bones[bone_index].name;
    }

    std::uint32_t BoneResolver::bone_count(void) const noexcept {
        return static_cast<std::uint32_t>(this->bone_data->bones.size());
    }
} // namespace enishi::assets_system