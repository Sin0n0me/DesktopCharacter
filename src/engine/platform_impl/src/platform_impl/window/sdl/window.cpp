#include "window.h"

namespace enishi::platform_impl {
    void SDLWindowDeleter::operator()(SDL_Window* const ptr) const {
        if (ptr) {
            SDL_DestroyWindow(ptr);
        }
    }

    types::WindowHandle SDLWindow::get_handle(void) const {
        return types::WindowHandle();
    }

    types::WindowPosition SDLWindow::get_position(void) const {
        return types::WindowPosition();
    }

    void SDLWindow::set_position(const types::WindowPosition position) {
    }

    types::WindowSize SDLWindow::get_size(void) const {
        return types::WindowSize();
    }

    void SDLWindow::set_size(const types::WindowSize size) {
    }

    void SDLWindow::set_title(const std::string& title) {
    }

    std::string SDLWindow::get_title(void) const {
        return std::string();
    }

    void SDLWindow::close(void) {
    }

    void SDLWindow::poll_events(void) {
    }
} // namespace enishi::platform_impl