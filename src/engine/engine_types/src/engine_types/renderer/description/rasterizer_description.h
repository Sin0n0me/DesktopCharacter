#pragma once
#include <cstdint>

namespace enishi::types {
    enum class FillMode : std::uint8_t {
        Solid,     // 塗りつぶし
        Wireframe, // ワイヤーフレーム
    };

    enum class CullMode : std::uint8_t {
        None,  // カリングなし
        Front, // 前面カリング
        Back,  // 背面カリング
    };

    enum class FrontFace : std::uint8_t {
        CounterClockwise, // 反時計回りが表(OpenGL, Vulkan標準)
        Clockwise,        // 時計回りが表(DirectX標準)
    };

    struct DepthBiasDescription {
        bool enable;
        float constant_factor; // 固定オフセット
        float slope_factor;    // 傾きに応じたオフセット
        float clamp;           // バイアスの最大値
    };

    struct RasterizerDescription {
        FillMode fill_mode;
        CullMode cull_mode;
        FrontFace front_face;
        bool depth_clamp; // 深度値をクランプ
        float line_width; // Wireframe時の線幅
        DepthBiasDescription depth_bias;

        [[nodiscard]]
        static constexpr RasterizerDescription default_solid(void) noexcept {
            return RasterizerDescription{
                .fill_mode = FillMode::Solid,
                .cull_mode = CullMode::Back,
                .front_face = FrontFace::CounterClockwise,
                .depth_clamp = false,
                .line_width = 1.0f,
                .depth_bias =
                    {
                        .enable = false,
                        .constant_factor = 0.0f,
                        .slope_factor = 0.0f,
                        .clamp = 0.0f,
                    },
            };
        }

        [[nodiscard]]
        static constexpr RasterizerDescription wireframe(void) noexcept {
            RasterizerDescription desc = RasterizerDescription::default_solid();
            desc.fill_mode = FillMode::Wireframe;
            desc.cull_mode = CullMode::None;
            return desc;
        }

        [[nodiscard]]
        static constexpr RasterizerDescription no_cull(void) noexcept {
            RasterizerDescription desc = RasterizerDescription::default_solid();
            desc.cull_mode = CullMode::None;
            return desc;
        }

        // シャドウマップ用
        [[nodiscard]]
        static constexpr RasterizerDescription shadow_map(void) noexcept {
            RasterizerDescription desc = RasterizerDescription::default_solid();
            desc.cull_mode = CullMode::Front;
            desc.depth_bias = {
                .enable = true,
                .constant_factor = 2.0f,
                .slope_factor = 1.5f,
                .clamp = 0.0f,
            };
            return desc;
        }
    };
} // namespace enishi::types