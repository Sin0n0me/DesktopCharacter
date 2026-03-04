#pragma once
#include "IMouseStateGetter.h"

struct MouseState : IMouseStateGetter {
public:
    POINT mouse_position;
    POINT pre_mouse_position;
    bool is_dragging;
    bool is_right_click;
    bool is_left_click;
    bool is_middle_click;

public:
    explicit MouseState(void) noexcept;

    bool init(const HWND& hwnd);
    void update(void);

    POINT get_mouse_position(void) const override;
    POINT get_pre_mouse_position(void) const override;
    bool get_is_dragging(void) const override;
    bool get_is_right_click(void) const override;
    bool get_is_left_click(void) const override;
    bool get_is_middle_click(void) const override;
    bool is_moved(void) const override;

    void add_position(const int x, const int y);
};
