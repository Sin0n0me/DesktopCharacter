#include "shadow_map_render_pass_constructor.h"
#include <foundation/log/logger.h>
#include <foundation/path/path_utility.h>
#include <foundation/str/string_builder.h>
#include <render_pass/constructor/helper.h>
#include <renderer/common/render_pass/render_pass.h>

namespace enishi::render_pass {
    constexpr char VS_FILE_NAME[] = "vs_shadow_map";
    constexpr char PS_FILE_NAME[] = "ps_shadow_map";
    constexpr std::uint32_t SHADOW_MAP_SIZE = 2048;

    foundation::Result<std::shared_ptr<platform::IRenderPass>, ConstructError>
    enishi::render_pass::ShadowMapRenderPassConstructor::make(
        platform::IRenderer* const renderer, const platform::IWindow* window) {
        auto render_pass = std::make_shared<renderer::RenderPass>();

        types::PipelineDescription description{
            .topology = types::PrimitiveTopology::TriangleList,
        };

        const auto opt_window_size = window->get_size();
        if (opt_window_size.is_none()) {
            return;
        }
        auto window_size = opt_window_size.unwrap().to_glm_ivec2();

        // レンダーターゲットの作成
        auto rtv = make_render_target(types::ImageDescription::make_render_target(window_size),
            types::ImageFormat::BGRA8_UNORM,
            renderer);
        if (rtv.is_err()) {
            return std::move(rtv).unwrap_err();
        }
        description.render_target_view = rtv.unwrap();

        // 深度ステンシルの作成
        auto dsv = make_depth_stencil(types::ImageDescription::make_depth_stencil(
                                          window_size, types::ImageFormat::D24_UNORM_S8_UINT),
            types::ImageFormat::BGRA8_UNORM,
            renderer);
        if (dsv.is_err()) {
            return std::move(dsv).unwrap_err();
        }
        description.depth_stencil_view = dsv.unwrap();

        // ラスタライザの作成
        auto rasterizer = make_rasterizer(
            types::RasterizerStateDescription{
                .cull_mode = types::CullMode::None,
                .front_face = types::FrontFace::CounterClockwise,
            },
            renderer);
        if (rasterizer.is_err()) {
            return std::move(rasterizer).unwrap_err();
        }
        description.rasterizer_state = rasterizer.unwrap();

        // レンダーパスの生成
        const auto render_pass_result =
            render_pass->make_render_pass(description, this->get_node(), this->get_dependencies());
        if (render_pass_result.is_err()) {
            return render_pass_result.propagation(ConstructError::Construct);
        }

        return render_pass;
    }

    foundation::DependencyNode ShadowMapRenderPassConstructor::get_node(void) const noexcept {
        return NODE;
    }

    foundation::DependencyBounds ShadowMapRenderPassConstructor::get_dependencies(
        void) const noexcept {
        return {};
    }

    void enishi::render_pass::ShadowMapRenderPassConstructor::import_shader(
        const types::ShaderKind& shader_kind, const types::ShaderData& shader) const noexcept {
        // シェーダーの作成
        auto shader_result = make_shader(shader_kind, shader, renderer)
                                 .add_message("シェーダーの作成に失敗しました");
        if (shader_result.is_err()) {
            return shader_result.propagation(ConstructError::Construct);
        }
        std::vector<types::RenderHandle> shader_refrections;
        for (auto& s : shader_result.unwrap()) {
            if (s.input_layout.is_valid()) {
                description.vertex_layout = s.input_layout;
            }
            description.shaders.emplace_back(s.shader);
            shader_refrections.emplace_back(s.shader_reflection);
        }
    }

    std::vector<std::tuple<types::ShaderKind, std::filesystem::path>>
    enishi::render_pass::ShadowMapRenderPassConstructor::get_paths(void) const noexcept {
        return {
            {types::ShaderKind::Vertex, VS_FILE_NAME},
            {types::ShaderKind::Pixel, PS_FILE_NAME},
        };
    }

    foundation::UTF8 enishi::render_pass::ShadowMapRenderPassConstructor::get_render_pass_name(
        void) const noexcept {
        return foundation::UTF8();
    }
} // namespace enishi::render_pass