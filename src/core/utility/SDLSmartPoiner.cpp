#include "SDLSmartPoiner.h"
#include <SDL3/SDL.h>

void SDLWindowDeleter::operator()(SDL_Window* const ptr) const {
    if(ptr) {
        SDL_DestroyWindow(ptr);
    }
}