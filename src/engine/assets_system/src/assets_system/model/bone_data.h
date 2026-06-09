#pragma once
#include <engine_types/assets/model/bone.h>
#include <glm/glm.hpp>
#include <string>

namespace enishi::assets_system {
    // このアプリケーション向けに設定されたモデルデータ
    //
    struct BoneNode {
        constexpr static types::BoneIndex ROOT_NODE_INDEX = 0;

        types::BoneIndex parent_index; //
    };
} // namespace enishi::assets_system
