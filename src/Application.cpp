#include "application.h"
#include <core/system/animation/animation_system.h>
#include <foundation/log/logger.h>
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
    bool Application::init(void) {
        const auto window_size = types::WindowSize{
            .width = 100,
            .height = 200,
        };
        this->root_window = std::make_unique<platform_impl::SDL3Window>(
            "enishi", window_size, platform::WindowSystem::Windows, types::GraphicsAPI::DirectX11);
        if (!bool(this->root_window)) {
            return false;
        }
        if (this->root_window->init().is_err()) {
            return false;
        }

        const auto opt_handle = this->root_window->get_handle();
        if (opt_handle.is_none()) {
            return false;
        }
        const auto& window_handle = opt_handle.value();

        auto renderer =
            renderer::directx::D3D11RenderInitializer{}.init(window_handle, window_size);
        if (renderer.is_err()) {
            return false;
        }

        this->renderer = std::move(renderer.value());

        this->rsegistory = std::make_shared<ecs::Registory>();

        // システムの追加
        this->system_scheduler.register_system<core::AssetManager>(50);
        this->system_scheduler.register_system<core::AnimationSystem>(80, this->rsegistory);
        this->render_system = this->system_scheduler.register_system<core::RenderSystem>(100);

        return true;
    }

    void Application::run(void) {
        const auto init_time = this->app_timer.tick_unclamp();
        foundation::Logger::info(std::format("初期化時間: {:%S}秒", init_time.delta_time));

        for (; this->root_window->should_close();) {
            this->root_window->poll_events();

            const auto dt = this->app_timer.tick();

            // 更新
            this->system_scheduler.update(dt);

            // 描画
            const auto& render_graph = this->render_system->get_render_graph();
            this->renderer->submit_render_graph(render_graph);
            this->renderer->present();
        }
    }
} // namespace enishi