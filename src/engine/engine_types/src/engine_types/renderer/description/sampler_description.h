#pragma once
#include <cstdint>

namespace enishi::types {
    enum class FilterMode : std::uint8_t {
        Nearest, // ニアレスト
        Linear,  // リニア
    };

    enum class AddressMode : std::uint8_t {
        Repeat, // タイリング
        Clamp,  // 端をクランプ
        Mirror, // ミラーリング
    };

    enum class AnisotropyLevel : std::uint8_t {
        None = 1,
        X2 = 2,
        X4 = 4,
        X8 = 8,
        X16 = 16,
    };

    struct SamplerDescription {
        FilterMode mag_filter;
        FilterMode min_filter;
        FilterMode mip_filter;
        AddressMode address_u;
        AddressMode address_v;
        AddressMode address_w;
        AnisotropyLevel anisotropy;
        float mip_lod_bias;
        float min_lod;
        float max_lod;

        [[nodiscard]]
        static constexpr SamplerDescription default_linear(void) noexcept {
            return SamplerDescription{
                .mag_filter = FilterMode::Linear,
                .min_filter = FilterMode::Linear,
                .mip_filter = FilterMode::Linear,
                .address_u = AddressMode::Repeat,
                .address_v = AddressMode::Repeat,
                .address_w = AddressMode::Repeat,
                .anisotropy = AnisotropyLevel::X4,
                .mip_lod_bias = 0.0f,
                .min_lod = 0.0f,
                .max_lod = 1000.0f,
            };
        }

        [[nodiscard]]
        static constexpr SamplerDescription default_nearest(void) noexcept {
            return SamplerDescription{
                .mag_filter = FilterMode::Nearest,
                .min_filter = FilterMode::Nearest,
                .mip_filter = FilterMode::Nearest,
                .address_u = AddressMode::Repeat,
                .address_v = AddressMode::Repeat,
                .address_w = AddressMode::Repeat,
                .anisotropy = AnisotropyLevel::None,
                .mip_lod_bias = 0.0f,
                .min_lod = 0.0f,
                .max_lod = 1000.0f,
            };
        }

        [[nodiscard]]
        static SamplerDescription clamp_linear(void) noexcept {
            return SamplerDescription{
                .mag_filter = FilterMode::Linear,
                .min_filter = FilterMode::Linear,
                .mip_filter = FilterMode::Linear,
                .address_u = AddressMode::Clamp,
                .address_v = AddressMode::Clamp,
                .address_w = AddressMode::Clamp,
                .anisotropy = AnisotropyLevel::X4,
                .mip_lod_bias = 0.0f,
                .min_lod = 0.0f,
                .max_lod = 1000.0f,
            };
        }
    };
} // namespace enishi::types