#pragma once
#include "../utility/SDLSmartPoiner.h"
#include "WindowStatus.h"
#include <optional>

class Collider;

class IWindowFrame {
public:
    virtual ~IWindowFrame(void) noexcept = default;

    virtual bool create(SDLWindowPtr ptr) = 0;
    virtual void on_update(const Collider* collider) = 0;
    virtual WindowStatus get_window_status(void) const;
    virtual WindowPoint get_window_size(void) const = 0;
    virtual WindowPoint get_screen_position(void) const = 0;
    virtual std::optional<SDL_Window*> get_window(void) const;
};
