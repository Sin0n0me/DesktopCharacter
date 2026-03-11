#include "WindowsWindowTranslator.h"
#include <SDL3/SDL.h>

HWND WindowsWindowTranslator::get_hwnd(SDL_Window* window) {
    SDL_PropertiesID props = SDL_GetWindowProperties(window);

    return (HWND)SDL_GetPointerProperty(
        props,
        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
        nullptr
    );
}