#pragma once
#include "image_view_description.h"
#include <cstdint>
#include <glm/glm.hpp>

namespace enishi::types {
    enum class ImageUsage : std::uint32_t {
        None = 0,
        RenderTarget = 1 << 0,    // RTV として使う
        DepthStencil = 1 << 1,    // DSV として使う
        ShaderResource = 1 << 2,  // SRV として使う
        UnorderedAccess = 1 << 3, // UAV として使う
        TransferSrc = 1 << 4,     // コピー元
        TransferDst = 1 << 5,     // コピー先
    };

    // ビット演算を使えるようにする
    [[nodiscard]]
    constexpr ImageUsage operator|(const ImageUsage& a, const ImageUsage& b) noexcept {
        return static_cast<ImageUsage>(
            static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
    }

    [[nodiscard]]
    constexpr bool has_usage(const ImageUsage& flags, const ImageUsage& bit) noexcept {
        return (static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(bit)) != 0;
    }

    struct ImageDescription {
        glm::ivec2 size;
        ImageFormat format;
        ImageUsage usage;
        std::uint32_t mipLevels = 1;
        std::uint32_t arrayLayers = 1;
        std::uint32_t samples = 1; // MSAA サンプル数

        [[nodiscard]]
        static constexpr ImageDescription make_render_target(
            const glm::ivec2& size, const ImageFormat format = ImageFormat::RGBA8_UNORM) noexcept {
            return ImageDescription{
                .size = size,
                .format = format,
                .usage = ImageUsage::RenderTarget | ImageUsage::ShaderResource,
            };
        }

        [[nodiscard]]
        static constexpr ImageDescription make_depth_stencil(
            const glm::ivec2& size, const ImageFormat format = ImageFormat::D32_FLOAT) noexcept {
            return ImageDescription{
                .size = size,
                .format = format,
                .usage = ImageUsage::DepthStencil | ImageUsage::ShaderResource,
            };
        }

        [[nodiscard]]
        static constexpr ImageDescription make_texture(const glm::ivec2& size,
            const ImageFormat format = ImageFormat::RGBA8_UNORM,
            const std::uint32_t mipLevels = 1) noexcept {
            return ImageDescription{
                .size = size,
                .format = format,
                .usage = ImageUsage::ShaderResource | ImageUsage::TransferDst,
                .mipLevels = mipLevels,
            };
        }
    };
} // namespace enishi::types