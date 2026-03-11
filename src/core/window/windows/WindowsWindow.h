#pragma once
#include "../IWindowFrame.h"

class WindowsWindow : public IWindowFrame {
private:
    SDLWindowPtr window;

public:
    explicit WindowsWindow(void);

    bool create(SDLWindowPtr ptr) override;
    void on_update(const Collider* collider) override;
    WindowPoint get_window_size(void) const override;
    WindowPoint get_screen_position(void) const override;
    std::optional<SDL_Window*> get_window(void) const override;
};
