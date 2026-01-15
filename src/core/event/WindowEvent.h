#pragma once
#include "../Core.h"
#include <Windows.h>

class WindowEvent {
private:
    static bool hit_model(const Engine* instance, const POINT& point);

    static LRESULT on_input(const Engine* instance, const HWND hwnd, const WPARAM wparam, const LPARAM lparam);

    static LRESULT on_mouse_move(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT on_mouse_left_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT on_mouse_left_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT on_mouse_right_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT on_mouse_right_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT on_hit_test(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);

public:

    static LRESULT CALLBACK input_wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT CALLBACK render_wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
};
