#pragma once
#include <windows.h>

class IMouseStateGetter {
public:
    virtual POINT get_mouse_position(void) const = 0;
    virtual POINT get_pre_mouse_position(void) const = 0;
    virtual bool get_is_dragging(void) const = 0;
    virtual bool get_is_right_click(void) const = 0;
    virtual bool get_is_left_click(void) const = 0;
    virtual bool get_is_middle_click(void) const = 0;
    virtual bool is_moved(void) const = 0;
};
