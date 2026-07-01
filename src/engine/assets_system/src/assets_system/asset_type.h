#pragma once
#include <cstdint>

namespace enishi::assets_system {
    enum class AssetType : std::uint8_t {
        Model,
        Animation,
        Shader,
        Texture,
        Video,
        Sound,
        Script,
    };
} // namespace enishi::assets_system
