#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <platform/handler/window/window.h>

namespace enishi::platform_impl {
    struct SDLWindowDeleter {
        void operator()(SDL_Window* const ptr) const;
    };

    using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

    class SDLWindow : public platform::IWindow {
      private:
        SDLWindowPtr window;

      public:
        types::WindowHandle get_handle(void) const override;
        types::WindowPosition get_position(void) const override;
        void set_position(const types::WindowPosition position) override;
        types::WindowSize get_size(void) const override;
        void set_size(const types::WindowSize size) override;
        void set_title(const std::string& title) override;
        std::string get_title(void) const override;
        void close(void) override;
        void poll_events(void) override;
    };
} // namespace enishi::platform_impl
