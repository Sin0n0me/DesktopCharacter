#pragma once
#include "IMouseStateGetter.h"

struct MouseState : IMouseStateGetter {
public:
	int screen_position_x;
	int screen_position_y;
	POINT mouse_position;
	POINT pre_mouse_position;
	bool is_dragging;
	bool is_right_click;
	bool is_left_click;
	bool is_middle_click;

public:

	int get_screen_position_x(void) const override;
	int get_screen_position_y(void) const override;
	POINT get_mouse_position(void) const override;
	POINT get_pre_mouse_position(void) const override;
	bool get_is_dragging(void) const override;
	bool get_is_right_click(void) const override;
	bool get_is_left_click(void) const override;
	bool get_is_middle_click(void) const override;
};
