#include "../../Application.h"
#include "../../config/GraphicsAPI.h"
#include "../collider/Collider.h"
#include "../log/Logger.h"
#include "IWindowFrame.h"
#include "WindowManager.h"
#include <SDL3/SDL.h>

constexpr char WINDOW_NAME[] = "ゆかりさんを自由に動かしたい";

WindowManager::WindowManager(
    std::unique_ptr<IWindowFrame> root_window
) :
    root_window(std::move(root_window)) {
}

WindowManager::~WindowManager(void) noexcept {
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool WindowManager::make_root_window(const GraphicsAPI& api) {
    const SDL_WindowFlags api_flag = [&api](void) -> SDL_WindowFlags {
        switch(api) {
        case GraphicsAPI::Vulkan10:
        case GraphicsAPI::Vulkan11:
        case GraphicsAPI::Vulkan12:
        case GraphicsAPI::Vulkan13:
        case GraphicsAPI::Vulkan14:
            return SDL_WINDOW_VULKAN;
        case GraphicsAPI::OpenGL40:
        case GraphicsAPI::OpenGL41:
        case GraphicsAPI::OpenGL42:
        case GraphicsAPI::OpenGL43:
        case GraphicsAPI::OpenGL44:
        case GraphicsAPI::OpenGL45:
            return SDL_WINDOW_OPENGL;
        default:
            break;
        }
        return 0;
        }();

    if(!this->root_window->create(
        SDLWindowPtr(
            SDL_CreateWindow(
                WINDOW_NAME,
                WIDTH,
                HEIGHT,
                SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS | api_flag
            )
        )
    )) {
        return false;
    }

    return true;
}

bool WindowManager::init(const GraphicsAPI& api) {
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        std::string err(SDL_GetError());
        Logger::error(std::u8string(err.begin(), err.end()));
        return false;
    }
    if(!SDL_InitSubSystem(SDL_INIT_EVENTS)) {
        std::string err(SDL_GetError());
        Logger::error(std::u8string(err.begin(), err.end()));
        return false;
    }

    if(!this->make_root_window(api)) {
        return false;
    }

    return true;
}

void WindowManager::update(const Collider* collider) {
    this->root_window->on_update(collider);
}

WindowStatus WindowManager::get_window_status(void) const {
    return this->root_window->get_window_status();
}

std::optional<SDL_Window*> WindowManager::get_window(void) const {
    return this->root_window->get_window();
}