#pragma once
#include <cstdint>
#include <vector>

namespace enishi::assets_system {
    enum class TextureFormat : std::uint32_t { RGBA8, BC7, BC3, R8 };

    struct TextureData {
        std::vector<uint8_t> pixels;
        uint32_t width;
        uint32_t height;
        TextureFormat format;
        uint32_t mip_levels;
    };
} // namespace enishi::assets_system