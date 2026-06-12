#include "sdl3_window.h"
#include <SDL3/SDL_init.h>
#include <platform/window/common_settings.h>

namespace enishi::platform_impl {
    void SDLWindowDeleter::operator()(SDL_Window* const ptr) const {
        if (ptr) {
            SDL_DestroyWindow(ptr);
        }
    }

    SDL3Window::SDL3Window(SDLWindowPtr window_ptr)
        : window(std::move(window_ptr)) {
    }

    foundation::EngineResult<SDL3Window, platform::WindowError> SDL3Window::make(
        const types::GraphicsAPI graphics_api) {
        const SDL_WindowFlags api_flag = SDL3Window::get_flag_from_graphics_api(graphics_api);

        return SDL3Window{SDLWindowPtr(SDL_CreateWindow(platform::ROOT_WINDOW_NAME,
            platform::WIDTH,
            platform::HEIGHT,
            SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS | api_flag))};
    }

    std::optional<const platform::IInput*> SDL3Window::get_input(void) const noexcept {
        return &this->input;
    }

    std::optional<SDL_Window*> SDL3Window::get_window_handle(void) const {
        if (!bool(this->window)) {
            return {};
        }
        return this->window.get();
    }

    SDL_WindowFlags SDL3Window::get_flag_from_graphics_api(const types::GraphicsAPI graphics_api) {
        switch (graphics_api) {
            case types::GraphicsAPI::Vulkan10:
            case types::GraphicsAPI::Vulkan11:
            case types::GraphicsAPI::Vulkan12:
            case types::GraphicsAPI::Vulkan13:
            case types::GraphicsAPI::Vulkan14:
                return SDL_WINDOW_VULKAN;
            case types::GraphicsAPI::OpenGL40:
            case types::GraphicsAPI::OpenGL41:
            case types::GraphicsAPI::OpenGL42:
            case types::GraphicsAPI::OpenGL43:
            case types::GraphicsAPI::OpenGL44:
            case types::GraphicsAPI::OpenGL45:
                return SDL_WINDOW_OPENGL;
            default:
                break;
        }
        return 0;
    }

    std::optional<types::WindowHandle> SDL3Window::get_handle(void) const noexcept {
        return std::optional<types::WindowHandle>();
    }

    std::optional<types::WindowPosition> SDL3Window::get_position(void) const noexcept {
        types::WindowPosition position{};
        if (!SDL_GetWindowPosition(this->window.get(), &position.x, &position.y)) {
            return {};
        }

        return position;
    }

    foundation::VoidResult<platform::WindowError> SDL3Window::set_position(
        const types::WindowPosition& position) noexcept {
        if (!SDL_SetWindowPosition(this->window.get(), position.x, position.y)) {
            return foundation::Error(platform::WindowError::FailedSetWinodwPosition);
        }

        return {};
    }

    std::optional<types::WindowSize> SDL3Window::get_size(void) const noexcept {
        types::WindowSize size{};
        if (!SDL_GetWindowSize(this->window.get(), &size.width, &size.height)) {
            return {};
        }

        return size;
    }

    foundation::VoidResult<platform::WindowError> SDL3Window::set_size(
        const types::WindowSize& size) noexcept {
        if (!SDL_SetWindowSize(this->window.get(), size.width, size.height)) {
            return foundation::Error(platform::WindowError::FailedSetWinodwSize);
        }

        return {};
    }

    foundation::VoidResult<platform::WindowError> SDL3Window::set_title(
        const std::string& title) noexcept {
        return {};
    }

    std::optional<std::string> SDL3Window::get_title(void) const noexcept {
        return std::optional<std::string>();
    }

    void SDL3Window::close(void) {
    }

    void SDL3Window::poll_events(void) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            /*
            if (ImGui_ImplSDL3_ProcessEvent(&event)) {
                continue;
            };
            */

            switch (event.type) {
                case SDL_EventType::SDL_EVENT_QUIT:
                    this->is_closing = true;
                    break;
                case SDL_EventType::SDL_EVENT_KEY_DOWN:
                    this->input.on_key_down(SDL3Input::convert_key_code(event.key.key));
                    break;
                case SDL_EventType::SDL_EVENT_KEY_UP:
                    this->input.on_key_up(SDL3Input::convert_key_code(event.key.key));
                    break;
                case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
                    this->input.on_mouse_button_down(SDL3Input::convert_mouse_button(event.));
                    break;
                case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
                    this->input.on_mouse_button_up(SDL3Input::convert_mouse_button(event.));
                    break;
                case SDL_EventType::SDL_EVENT_MOUSE_MOTION:
                    this->input.on_mouse_move();
                    break;
                case SDL_EventType::SDL_EVENT_MOUSE_WHEEL:

                    break;
                case SDL_EventType::SDL_EVENT_WINDOW_FOCUS_GAINED:
                    // SDL_StartTextInput();
                    break;
                default:
                    break;
            }
        }

        SDL_Keymod mod = SDL_GetModState();
        /*
        io.AddKeyEvent(ImGuiKey::ImGuiKey_LeftCtrl, (mod & SDL_KMOD_CTRL) != 0);
        io.AddKeyEvent(ImGuiKey::ImGuiKey_LeftShift, (mod & SDL_KMOD_SHIFT) != 0);
        io.AddKeyEvent(ImGuiKey::ImGuiMod_Alt, (mod & SDL_KMOD_ALT) != 0);
        */
    }
} // namespace enishi::platform_impl