#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace enishi::types {
    enum class TextureFormat : std::uint32_t {
        RGBA8,
        BC7,
        BC3,
        R8,
    };

    struct TextureData {
        TextureFormat format;
        std::vector<std::uint8_t> pixels;
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t mip_levels;
    };
} // namespace enishi::types