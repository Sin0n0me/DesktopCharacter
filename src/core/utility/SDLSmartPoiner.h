#pragma once
#include <memory>

struct SDL_Window;

struct SDLWindowDeleter {
    void operator()(SDL_Window* const ptr) const;
};

using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;
