#pragma once
#include <engine_types/assets/model/mesh_data.h>
#include <engine_types/assets/texture/texture_data.h>
#include <foundation/str/str.h>

namespace enishi::assets_system {
    // このアプリケーション向けに設定されたモデルデータ
    //
    struct ModelData {
        foundation::UTF8 name;
    };
} // namespace enishi::assets_system
