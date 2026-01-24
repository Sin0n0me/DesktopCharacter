#pragma once
#include "../Core.h"
#include <Windows.h>

class WindowEvent {
private:
    static HCURSOR cursor;

    static LRESULT on_input(const Engine* instance, const HWND hwnd, const WPARAM wparam, const LPARAM lparam);
    static void on_mouse_input(const Engine* instance, const RAWMOUSE& raw_mouse, const HWND hwnd, const WPARAM wparam, const LPARAM lparam);
    static void on_mouse_move(const Engine* instance, const LONG dx, const LONG dy, const HWND hwnd, const WPARAM wparam, const LPARAM lparam);
    static void on_mouse_click_left(const Engine* instance, const bool is_up, const HWND hwnd, const WPARAM wparam, const LPARAM lparam);
    //static void on_mouse_click_right(const Engine* instance, const bool is_up, const HWND hwnd, const WPARAM wparam, const LPARAM lparam);
    //static void on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);

public:

    static LRESULT CALLBACK wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
    static LRESULT CALLBACK input_wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
};
