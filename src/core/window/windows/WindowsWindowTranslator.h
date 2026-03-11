#pragma once
#include <wtypes.h>

struct SDL_Window;

class WindowsWindowTranslator {
private:
    explicit WindowsWindowTranslator(void) = delete;

public:
    static HWND get_hwnd(SDL_Window* window);
};
