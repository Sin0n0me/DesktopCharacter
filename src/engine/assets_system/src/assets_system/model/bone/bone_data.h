#pragma once
#include <engine_types/assets/model/bone.h>
#include <foundation/option/option.h>
#include <foundation/str/str.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

namespace enishi::assets_system {
    struct Bone {
        foundation::UTF8 name;
    };

    struct BoneData {
        std::vector<Bone> bones;

        // 名前 -> インデックスの対応表
        // ロード時に一度だけ構築, 以降は不変
        std::unordered_map<foundation::UTF8, types::BoneIndex> bone_name_to_index;

        // ロード完了後に呼ぶ
        void build_name_index(void);

        // 名前からインデックスを引く
        [[nodiscard]]
        foundation::Option<types::BoneIndex> find_bone_index(const foundation::UTF8& name) const;
    };
} // namespace enishi::assets_system
