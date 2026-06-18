#include "bone_data.h"

namespace enishi::assets_system {
    void BoneData::build_name_index(void) {
        this->bone_name_to_index.clear();
        for (uint32_t i = 0; i < static_cast<uint32_t>(this->bones.size()); ++i) {
            this->bone_name_to_index[this->bones[i].name] = i;
        }
    }

    foundation::Option<types::BoneIndex> assets_system::BoneData::find_bone_index(
        const foundation::UTF8& name) const {
        const auto it = this->bone_name_to_index.find(name);
        if (it == this->bone_name_to_index.end()) {
            return {};
        }
        return it->second;
    }
} // namespace enishi::assets_system