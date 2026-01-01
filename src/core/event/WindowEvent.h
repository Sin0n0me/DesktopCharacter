#pragma once
#include <Windows.h>
#include "../Core.h"

class WindowEvent {
private:
	static HWND target_hwnd;
	static HWND self_hwnd;

	static bool hit_model(const Engine* instance, const POINT& point);
	static LRESULT forward_message_to_underlying_window(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_move(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_left_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_left_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_right_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_right_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT on_hit_test(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);

	static void snap_to_top_edge(void);
public:

	static LRESULT CALLBACK wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam);

	static void CALLBACK hook_win_event(
		const HWINEVENTHOOK,
		const DWORD event,
		const HWND hwnd,
		const LONG idObject,
		const LONG,
		const DWORD,
		const DWORD
	);
};
