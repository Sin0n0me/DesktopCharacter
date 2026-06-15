#pragma once
#include "../../input/sdl/sdl_input.h"
#include <SDL3/SDL.h>
#include <engine_types/renderer/graphics_api.h>
#include <foundation/option/option.h>
#include <foundation/result/result.h>
#include <memory>
#include <platform/window/window.h>

namespace enishi::platform_impl {
    struct SDLWindowDeleter {
        void operator()(SDL_Window* const ptr) const;
    };

    using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

    class SDL3Window : public platform::IWindow {
      private:
        const platform::WindowSystem window_system;
        SDLWindowPtr window;
        SDL3Input input;
        types::WindowSize size;
        types::WindowPosition position;
        bool is_closing;

        static SDL_WindowFlags get_flag_from_graphics_api(const types::GraphicsAPI graphics_api);
        static foundation::Option<platform::NativeWindowHandle> get_native_handle(
            SDL_Window* const window, const platform::WindowSystem window_system);

        explicit SDL3Window(void) = default;
        explicit SDL3Window(const platform::WindowSystem window_system, SDLWindowPtr window_ptr);

      public:
        static foundation::EngineResult<SDL3Window, platform::WindowError> make(
            const platform::WindowSystem window_system, const types::GraphicsAPI graphics_api);

        std::optional<SDL_Window*> get_window_handle(void) const;

        std::optional<const platform::IInput*> get_input(void) const noexcept override;
        std::optional<platform::WindowHandle> get_handle(void) const noexcept override;
        std::optional<types::WindowPosition> get_position(void) const noexcept override;
        foundation::VoidResult<platform::WindowError> set_position(
            const types::WindowPosition& position) noexcept override;
        std::optional<types::WindowSize> get_size(void) const noexcept override;
        foundation::VoidResult<platform::WindowError> set_size(
            const types::WindowSize& size) noexcept override;
        foundation::VoidResult<platform::WindowError> set_title(
            const std::string& title) noexcept override;
        std::optional<std::string> get_title(void) const noexcept override;
        void close(void) override;
        void poll_events(void) override;
    };
} // namespace enishi::platform_impl
