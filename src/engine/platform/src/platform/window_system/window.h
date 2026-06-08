#pragma once
#include <SDL3/SDL.h>
#include <memory>

namespace platform {
    namespace window {
        struct SDLWindowDeleter {
            void operator()(SDL_Window* const ptr) const;
        };

        using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;
    } // namespace window
} // namespace platform