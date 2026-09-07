#include "application.h"
#include <core/system/animation/animation_system.h>
#include <core/system/asset/asset_system.h>
#include <core/system/physics/physics_system.h>
#include <foundation/log/logger.h>
#include <foundation/str/string_builder.h>
#include <platform_impl/physics/physics_config.h>
#include <render_pass/constructor/back_ground/back_ground_render_pass_constructor.h>
// #include <render_pass/constructor/debug/debug_render_pass_constructor.h>
#include <render_pass/constructor/model/model_render_pass_constructor.h>
#include <render_pass/constructor/shadow/shadow_map_render_pass_constructor.h>
#include <render_pass/render_pass_constructor.h>

#include <physics/bullet3/physics_engine.h>

#include <platform_impl/window/sdl/sdl3_window.h>
#include <renderer/directx/directx11/d3d11_render_initializer.h>
#include <renderer/directx/directx11/d3d11_renderer.h>

int main(void) {
    enishi::Application app;

    if (!app.init()) {
        return -1;
    }

    app.run();

    return 0;
}

namespace enishi {
    constexpr auto INIT_WINDOW_SIZE = types::WindowSize{
        .width = WINDOW_SIZE.x,
        .height = WINDOW_SIZE.y,
    };

    bool Application::init(void) {
        this->rsegistory = std::make_shared<ecs::Registory>();

        // システムの追加
        auto asset_system = this->system_scheduler.register_system<core::AssetSystem>(50);
        auto animation_system =
            this->system_scheduler.register_system<core::AnimationSystem>(80, this->rsegistory);

        auto physics_engine = std::make_shared<physics::bullet3::PhysicsEngine>(
            std::make_shared<platform_impl::PhysicsWorldConfig>());

        auto physics_system = this->system_scheduler.register_system<core::PhysicsSystem>(
            90, this->rsegistory, physics_engine);

        // ウィンドウの初期化
        const auto root_window = this->init_window();
        if (!bool(root_window)) {
            return false;
        }
        foundation::Logger::info("ウィンドウの初期化に成功しました");

        // レンダラーの初期化
        const auto renderer = this->init_renderer(root_window, asset_system->get_asset_system());
        if (!bool(renderer)) {
            return false;
        }
        foundation::Logger::info("レンダラーの初期化に成功しました");

        this->init_physics(asset_system->get_asset_system(), physics_engine);

        return true;
    }

    void Application::run(void) {
        const auto init_time = this->app_timer.tick_unclamp();
        foundation::Logger::info(std::format("初期化時間: {:%S}s", init_time.delta_time));

        for (; !this->system_scheduler.should_close();) {
            const auto dt = this->app_timer.tick();

            // 更新
            this->system_scheduler.pre_update();
            this->system_scheduler.update(dt);
            this->system_scheduler.post_update();

            // 描画
            this->system_scheduler.render();
        }
    }

    std::shared_ptr<platform::IWindow> Application::init_window(void) {
        const auto window_manager = this->system_scheduler.register_system<core::WindowManager>(80,
            std::make_shared<platform_impl::SDL3Window>(APPLICATION_NAME,
                INIT_WINDOW_SIZE,
                platform::WindowSystem::Windows,
                types::GraphicsAPI::DirectX11));

        auto root_window = window_manager->get_root_window().lock();
        if (!bool(root_window)) {
            return {};
        }

        if (root_window->init().is_err()) {
            return {};
        }

        return root_window;
    }

    std::shared_ptr<platform::IRenderer> Application::init_renderer(
        std::shared_ptr<platform::IWindow> root_window,
        std::shared_ptr<assets_system::IAssetSystem> asset_system) {
        if (!bool(root_window)) {
            return {};
        }

        const auto opt_window_handle = root_window->get_handle();
        if (opt_window_handle.is_none()) {
            return {};
        }

        auto initializer = renderer::directx::D3D11RenderInitializer{};
        auto result_renderer = initializer.init(opt_window_handle.unwrap(), INIT_WINDOW_SIZE);
        if (result_renderer.is_err()) {
            return {};
        }

        auto&& renderer = result_renderer.unwrap();

        const auto rect = types::ViewportRect{
            .left_top_x = 0.0,
            .left_top_y = 0.0,
            .width = static_cast<float>(WINDOW_SIZE.x),
            .height = static_cast<float>(WINDOW_SIZE.y),
            .min_depth = 0.0,
            .max_depth = 1.0,
        };

        if (renderer->create_viewport(rect).is_err()) {
            return {};
        }

        const auto render_system = this->system_scheduler.register_system<core::RenderSystem>(
            100, this->rsegistory, renderer, renderer);

        // レンダーパスの作成
        render_pass::RenderPassConstructor constructor;
        constructor.add_render_pass_constructor(
            std::make_shared<render_pass::ModelRenderPassConstructor>());
        constructor.add_render_pass_constructor(
            std::make_shared<render_pass::BackGroundRenderPassConstructor>());
        constructor.add_render_pass_constructor(
            std::make_shared<render_pass::ShadowMapRenderPassConstructor>());

        constructor.use_asset_paths();

        // 一括構築
        auto&& result_passes = constructor.create_render_passes(
            {
                render_pass::ModelRenderPassConstructor::RENDER_PASS_NAME,
            },
            render_system->get_renderer().get());
        if (result_passes.is_err()) {
            foundation::Logger::error(result_passes.unwrap_err().get_message());
            return {};
        }

        // レンダーパスのセット
        render_system->set_render_passes(std::move(result_passes).unwrap_mut());

        return renderer;
    }

    void Application::init_physics(std::shared_ptr<assets_system::IAssetSystem> asset_system,
        std::shared_ptr<platform::IPhysicsEngine> physics_engine) {
        physics_engine->init_world();
    }

    foundation::Result<std::tuple<foundation::UTF8, types::RenderHandle>,
        render_pass::ConstructError>
    ModelRenderPassConstructor::make_mesh(platform::IRenderer* const renderer,
        const std::vector<types::RenderHandle>& shader_reflections) {
        const auto pattern_model_extensions =
            asset_system->get_extensions_pattern(types::AssetKind::Model);
        const auto path = MODEL_PATH / "";
        const std::regex pattern(
            std::format("{}.*{}", foundation::path_to_regex_str(path), pattern_model_extensions));
        const auto model_paths = asset_system->find_assets(MODEL_PATH, types::AssetKind::Model);
        const auto asset_paths = model_paths.find(pattern);

        if (asset_paths.empty()) {
            return foundation::Error(ConstructError::Construct, "モデルデータが見つかりません");
        }

        // モデルからメッシュへ変換
        foundation::StringBuilder error_message;
        for (const auto& path : asset_paths) {
            error_message.push_back(std::format("loaded path: {}", path.string<char>()));
            const auto asset_handle = asset_system->load_asset(path);
            if (asset_handle.is_err()) {
                error_message.push_back(asset_handle.unwrap_err().get_message());
                continue;
            }
            const auto opt_model_data = asset_system->get_model_data(asset_handle.unwrap());
            if (opt_model_data.is_none()) {
                continue;
            }
            const auto& model_data = opt_model_data.unwrap();

            // 先にテクスチャ読み込み
            for (const auto& material : model_data->materials) {
                for (const auto& material_texture : material.textures) {
                    const auto asset_handle = asset_system->load_asset(material_texture.path);
                    if (asset_handle.is_err()) {
                        error_message.push_back(asset_handle.unwrap_err().get_message());
                    }
                }
            }

            // メッシュ作成
            const auto mesh_handle = renderer->create_mesh(*model_data, shader_reflections);
            if (mesh_handle.is_err()) {
                error_message.push_back(mesh_handle.unwrap_err().get_message());
                continue;
            }

            return std::tuple{
                model_data->name,
                mesh_handle.unwrap(),
            };
        }

        return foundation::Error(ConstructError::Construct, error_message.join("\n"));
    }

    /*
    std::shared_ptr<types::ShaderData> get_shaders(
        assets_system::IAssetSystem* asset_system, const std::filesystem::path& path) {
        const auto shader_paths = asset_system->find_assets(SHADER_PATH, types::AssetKind::Shader);
        const auto pattern_shader_extensions =
            asset_system->get_extensions_pattern(types::AssetKind::Shader);
        const auto make_paths = [&](const std::filesystem::path& file_path) {
            const auto str_pattern = std::format(
                "{}{}", foundation::path_to_regex_str(file_path), pattern_shader_extensions);
            const std::regex pattern(str_pattern);
            return shader_paths.find(pattern);
        };
    }

    std::shared_ptr<types::ShaderData> get_shader(
        assets_system::IAssetSystem* asset_system, const std::filesystem::path& path) {
        const auto asset_handle =
            asset_system->load_asset(path).add_message("シェーダーの読み込みに失敗しました");
        if (asset_handle.is_err()) {
            return asset_handle.propagation(ConstructError::Construct);
        }
        const auto shader_data = asset_system->get_shader_data(asset_handle.unwrap());
        if (shader_data.is_none()) {
            return foundation::Error(ConstructError::Construct, "シェーダーデータが存在しません");
        }
        shader_data.unwrap();
    }
    */
} // namespace enishi