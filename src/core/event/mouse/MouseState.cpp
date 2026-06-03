#include "MouseState.h"
#include <Windows.h>

MouseState::MouseState(void) noexcept {
    this->is_dragging = false;
    this->is_left_click = false;
    this->is_middle_click = false;
    this->is_right_click = false;
    this->mouse_position = WindowPoint{};
}

bool MouseState::init(const HWND& hwnd) {
    POINT pt;
    if(!GetCursorPos(&pt)) {
        return false;
    }
    WindowPoint wpt{.x = pt.x, .y = pt.y};
    this->mouse_position = wpt;
    this->pre_mouse_position = wpt;

    return true;
}

void MouseState::update(void) {
    this->pre_mouse_position = this->mouse_position;

    POINT pt;
    if(!GetCursorPos(&pt)) {
        return;
    }
    WindowPoint wpt{.x = pt.x, .y = pt.y};
    this->mouse_position = wpt;
}

WindowPoint MouseState::get_mouse_position(void) const {
    return this->mouse_position;
}

WindowPoint MouseState::get_pre_mouse_position(void) const {
    return this->pre_mouse_position;
}

bool MouseState::get_is_dragging(void) const {
    return this->is_dragging;
}

bool MouseState::get_is_right_click(void) const {
    return this->is_right_click;
}

bool MouseState::get_is_left_click(void) const {
    return this->is_left_click;
}

bool MouseState::get_is_middle_click(void) const {
    return this->is_middle_click;
}

bool MouseState::is_moved(void) const {
    return this->pre_mouse_position.x != this->mouse_position.x
        && this->pre_mouse_position.y != this->mouse_position.y;
}

void MouseState::add_position(const int x, const int y) {
    this->mouse_position.x += x;
    this->mouse_position.y += y;
}