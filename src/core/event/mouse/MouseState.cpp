#include "MouseState.h"

int MouseState::get_screen_position_x(void) const {
	return this->screen_position_x;
}

int MouseState::get_screen_position_y(void) const {
	return this->screen_position_y;
}

POINT MouseState::get_mouse_position(void) const {
	return this->mouse_position;
}

POINT MouseState::get_pre_mouse_position(void) const {
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