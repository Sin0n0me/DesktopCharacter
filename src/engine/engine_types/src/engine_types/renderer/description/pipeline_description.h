#pragma once
#include "../render_handle.h"
#include "../vertex/vertex_layout.h"
#include "rasterizer_description.h"
#include <cstdint>

namespace enishi::types {
    enum class BlendMode : std::uint8_t {
        Opaque,        // 不透明
        AlphaBlend,    // 通常アルファブレンド
        Additive,      // 加算合成
        Premultiplied, // 乗算済みアルファ
    };

    enum class DepthTestMode : std::uint8_t {
        ReadWrite, // 深度テストあり, 書き込みあり(通常)
        ReadOnly,  // 深度テストあり, 書き込みなし(半透明)
        Disabled,  // 深度テストなし(UI, デバッグ)
    };

    enum class PrimitiveTopology : std::uint8_t {
        TriangleList, // 三角形リスト
        LineList,     // 線分リスト
        PointList,    // 点リスト
    };

    struct PipelineDescription {
        RenderHandle vertex_shader;
        RenderHandle fragment_shader;
        VertexLayout vertex_layout;
        RasterizerDescription rasterizer;
        BlendMode blend_mode;
        DepthTestMode depth_test;
        PrimitiveTopology topology;

        [[nodiscard]]
        static constexpr PipelineDescription make_opaque(
            RenderHandle vs, RenderHandle fs, const VertexLayout& layout) noexcept {
            return PipelineDescription{
                .vertex_shader = vs,
                .fragment_shader = fs,
                .vertex_layout = layout,
                .rasterizer = RasterizerDescription::default_solid(),
                .blend_mode = BlendMode::Opaque,
                .depth_test = DepthTestMode::ReadWrite,
                .topology = PrimitiveTopology::TriangleList,
            };
        }

        [[nodiscard]]
        static PipelineDescription make_shadow(
            RenderHandle vs, RenderHandle fs, const VertexLayout& layout) noexcept {
            return PipelineDescription{
                .vertex_shader = vs,
                .fragment_shader = fs,
                .vertex_layout = layout,
                .rasterizer = RasterizerDescription::shadow_map(),
                .depth_test = DepthTestMode::ReadWrite,
                .topology = PrimitiveTopology::TriangleList,
            };
        }

        [[nodiscard]]
        static PipelineDescription make_wireframe(
            RenderHandle vs, RenderHandle fs, const VertexLayout& layout) noexcept {
            return PipelineDescription{
                .vertex_shader = vs,
                .fragment_shader = fs,
                .vertex_layout = layout,
                .rasterizer = RasterizerDescription::wireframe(),
                .depth_test = DepthTestMode::ReadOnly,
                .topology = PrimitiveTopology::TriangleList,
            };
        }
    };
} // namespace enishi::types