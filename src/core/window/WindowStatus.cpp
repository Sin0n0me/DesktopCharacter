#include "WindowStatus.h"

WindowStatus::WindowStatus(
    const WindowPoint& screen_position,
    const WindowPoint& window_size
) noexcept :
    screen_position(screen_position),
    window_size(window_size) {
}

WindowPoint WindowStatus::get_screen_position(void) const noexcept {
    return this->screen_position;
}

WindowPoint WindowStatus::get_window_size(void) const noexcept {
    return this->window_size;
}

WindowPoint WindowStatus::to_client_position(const WindowPoint& pt) const noexcept {
    return WindowPoint{
        .x = pt.x - this->screen_position.x,
        .y = pt.y - this->screen_position.y,
    };
}

WindowPoint WindowStatus::to_screen_position(const WindowPoint& pt) const noexcept {
    return WindowPoint{
        .x = pt.x + this->screen_position.x,
        .y = pt.y + this->screen_position.y,
    };
}

void WindowStatus::update_screen_position(const WindowPoint& pt) {
    this->screen_position = pt;
}

void WindowStatus::update_window_size(const WindowPoint& pt) {
    this->window_size = pt;
}