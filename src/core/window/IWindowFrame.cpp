#include "IWindowFrame.h"

WindowStatus IWindowFrame::get_window_status(void) const {
    return WindowStatus(
        this->get_screen_position(),
        this->get_window_size()
    );
}

std::optional<SDL_Window*> IWindowFrame::get_window(void) const {
    return std::nullopt;
}