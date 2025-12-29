#pragma once
#include <Windows.h>
#include "../Core.h"

class WindowEvent {
private:
	static bool hit_model(const Engine* instance, const POINT& point);
	static LRESULT forward_message_to_underlying_window(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_move(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_left_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_left_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_right_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_right_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
public:

	static LRESULT CALLBACK wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
};
