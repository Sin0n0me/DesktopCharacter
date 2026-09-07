#include "model_render_pass_constructor.h"
#include <foundation/log/logger.h>
#include <foundation/path/path_utility.h>
#include <foundation/str/string_builder.h>
#include <render_pass/constructor/back_ground/back_ground_render_pass_constructor.h>
#include <render_pass/constructor/shadow/shadow_map_render_pass_constructor.h>
#include <renderer/common/render_pass/render_pass.h>

#include <platform/window/interface_window.h>

namespace enishi::render_pass {
    const std::filesystem::path SHADER_PATH = "./assets/shader";
    const std::filesystem::path MODEL_PATH = "./assets/models";
    constexpr char VS_FILE_NAME[] = "vs_model";
    constexpr char PS_FILE_NAME[] = "ps_model";

    foundation::Result<std::shared_ptr<platform::IRenderPass>, ConstructError>
    enishi::render_pass::ModelRenderPassConstructor::make(
        platform::IRenderer* const renderer, const platform::IWindow* window) {
        auto render_pass = std::make_shared<renderer::RenderPass>();

        types::PipelineDescription description{
            .topology = types::PrimitiveTopology::TriangleList,
        };

        const auto opt_window_size = window->get_size();
        if (opt_window_size.is_none()) {
            return;
        }
        const auto window_size = opt_window_size.unwrap().to_glm_ivec2();

        // レンダーターゲットの作成
        auto rtv =
            make_render_target(types::ImageDescription::make_default_render_target(window_size),
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

        // シェーダーの作成
        ShaderPaths paths = {
            {types::ShaderKind::Vertex, {VS_FILE_NAME}},
            {types::ShaderKind::Pixel, {PS_FILE_NAME}},
        };
        auto shader_result =
            make_shaders(renderer, std::move(paths)).add_message("シェーダーの作成に失敗しました");
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

        // レンダーパスの生成
        const auto render_pass_result = render_pass->make_render_pass(description);
        if (render_pass_result.is_err()) {
            return render_pass_result.propagation(ConstructError::Construct);
        }

        // モデルのみ初期モデル追加
        // TODO: ファイルからの読み取り初期モデルを選択するように
        const auto mesh_result = this->make_mesh(renderer, shader_refrections)
                                     .add_message("メッシュデータの作成に失敗しました");
        if (mesh_result.is_err()) {
            return mesh_result.propagation(ConstructError::Construct);
        }
        const auto& [name, mesh_handle] = mesh_result.unwrap();
        render_pass->add_mesh(name, mesh_handle);

        const auto& mapper = renderer->get_handle_mapper();
        auto accessor = renderer->get_resource_accessor()->get_resource_accessor();

        const auto opt_mapped_mesh_handle = mapper->get(mesh_handle);
        if (opt_mapped_mesh_handle.is_none()) {
            return foundation::Error(ConstructError::Construct);
        }
        const auto& mapped_mesh_handle = opt_mapped_mesh_handle.unwrap();
        const auto opt_mesh_handles =
            accessor->get_mesh_accessor()->get_mesh_handle(mapped_mesh_handle.resource);
        if (opt_mesh_handles.is_none()) {
            return foundation::Error(ConstructError::Construct);
        }

        // メッシュのバッファを外部から更新できるようにインターフェイスの取得
        const auto& mesh = opt_mesh_handles.unwrap();
        for (const auto& handle : mesh.mesh_handles) {
            const auto opt_buffer_handle = mapper->get(handle);
            if (opt_buffer_handle.is_none()) {
                return foundation::Error(ConstructError::Construct);
            }
            const auto& buffer_handle = opt_buffer_handle.unwrap();

            const auto& opt_buffer_interface =
                accessor->get_buffer_accessor()->get_bufer(buffer_handle.configurable);
            if (opt_buffer_interface.is_none()) {
                continue;
            }

            render_pass->add_updater(opt_buffer_interface.unwrap());
        }

        return render_pass;
    }

    foundation::DependencyNode ModelRenderPassConstructor::get_node(void) const noexcept {
        return NODE;
    }

    foundation::DependencyBounds ModelRenderPassConstructor::get_dependencies(void) const noexcept {
        return foundation::DependencyBounds{.precedents = {
                                                ShadowMapRenderPassConstructor::NODE,
                                                BackGroundRenderPassConstructor::NODE,
                                            }};
    }

    void enishi::render_pass::ModelRenderPassConstructor::import_shader(
        const types::ShaderKind& shader_kind, const types::ShaderData& shader) const noexcept {
    }
    std::vector<std::filesystem::path> enishi::render_pass::ModelRenderPassConstructor::get_paths(
        void) const noexcept {
        return std::vector<std::filesystem::path>();
    }
    foundation::UTF8 enishi::render_pass::ModelRenderPassConstructor::get_render_pass_name(
        void) const noexcept {
        return foundation::UTF8();
    }
} // namespace enishi::render_pass