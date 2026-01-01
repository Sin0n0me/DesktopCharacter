#include "WindowEvent.h"
#include "../../Application.h"

decltype(WindowEvent::target_hwnd) WindowEvent::target_hwnd;
decltype(WindowEvent::self_hwnd) WindowEvent::self_hwnd;

bool WindowEvent::hit_model(const Engine* instance, const POINT& point) {
	const Ray ray = Ray::make_ray_from_screen(
		static_cast<float>(point.x),
		static_cast<float>(point.y),
		instance->scene->get_camera().camera
	);

	for(const auto& pair : instance->models->get_obb_map()) {
		const auto& obb = pair.second;
		if(instance->collider->hit_model(ray, obb)) {
			return true;
		}
	}

	return false;
}

LRESULT WindowEvent::forward_message_to_underlying_window(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	// 一時的に透過
	/*
	SetWindowLong(
		hwnd,
		GWL_EXSTYLE,
		GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT
	);
	*/

	// 下のウィンドウを取得
	/*
	*/
	const HWND below = [&hwnd]() -> HWND {
		HWND below = GetWindow(hwnd, GW_HWNDNEXT);
		for(; below != NULL; below = GetWindow(below, GW_HWNDNEXT)) {
			if(IsWindowVisible(below) == FALSE || GetWindow(below, GW_OWNER) != NULL) {
				continue;
			}

			char title[256];
			GetWindowTextA(below, title, sizeof(title));
			if(strlen(title) > 0) {
				break;
			}
		}

		return below == NULL ? GetDesktopWindow() : below;
		}();

	POINT pt{};
	pt.x = LOWORD(lparam);
	pt.y = HIWORD(lparam);

	//const HWND below = WindowFromPoint(pt);
	if(!below || below == hwnd) {
		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	// 下のウィンドウのクライアント座標へ変換

	ClientToScreen(hwnd, &pt);
	ScreenToClient(below, &pt);
	const LPARAM new_lparam = MAKELPARAM(pt.x, pt.y);

	PostMessage(below, message, wparam, new_lparam);

	// 元に戻す
	/*
	SetWindowLong(
		hwnd,
		GWL_EXSTYLE,
		GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT
	);
	*/

	/*
	const HWND below2 = WindowFromPoint(pt);
	LONG oldStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, oldStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	SetWindowLong(hwnd, GWL_EXSTYLE, oldStyle);
	*/

	//return HTTRANSPARENT;
	//return result;
	return HTNOWHERE;
}

LRESULT WindowEvent::on_mouse_move(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	POINT pt{};
	pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
	pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標

	if(!WindowEvent::hit_model(instance, pt)) {
		return WindowEvent::forward_message_to_underlying_window(hwnd, message, wparam, lparam);
	}

	if(instance->mouse_state->is_dragging) {
		const int diff_x = pt.x - instance->mouse_state->pre_mouse_position.x;
		const int diff_y = pt.y - instance->mouse_state->pre_mouse_position.y;
		instance->mouse_state->screen_position_x += diff_x;
		instance->mouse_state->screen_position_y += diff_y;
		MoveWindow(
			hwnd,
			instance->mouse_state->screen_position_x,
			instance->mouse_state->screen_position_y,
			WIDTH,
			HEIGHT,
			FALSE
		);

		// ウィンドウ自体も移動しているため補正(移動先でまた移動した判定になるので荒ぶる)
		pt.x -= diff_x;
		pt.y -= diff_y;
	}

	instance->mouse_state->pre_mouse_position = pt;

	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_left_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	instance->mouse_state->is_dragging = false;
	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_left_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	POINT pt{};
	pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
	pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標
	if(!WindowEvent::hit_model(instance, pt)) {
		return WindowEvent::forward_message_to_underlying_window(hwnd, message, wparam, lparam);
	}

	instance->mouse_state->is_dragging = true;
	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_right_button_up(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_right_button_down(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	POINT pt{};
	pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
	pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標
	if(!WindowEvent::hit_model(instance, pt)) {
		return WindowEvent::forward_message_to_underlying_window(hwnd, message, wparam, lparam);
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_hit_test(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	/*
	*/
	POINT pt{};
	pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
	pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標

	return HTTRANSPARENT;
	if(!WindowEvent::hit_model(instance, pt)) {
		//return WindowEvent::forward_message_to_underlying_window(hwnd, message, wparam, lparam);
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}

void WindowEvent::snap_to_top_edge(void) {
	RECT rc;
	if(!GetWindowRect(WindowEvent::target_hwnd, &rc)) {
		return;
	}

	const int x = rc.left;           // 左揃え
	const int y = rc.top - HEIGHT;   // 上辺にスナップ

	SetWindowPos(
		WindowEvent::self_hwnd,
		nullptr,
		x,
		y,
		WIDTH,
		HEIGHT,
		SWP_NOZORDER | SWP_NOACTIVATE
	);
}

LRESULT WindowEvent::wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	if(!Engine::instance.has_value()) {
		if(message == WM_DESTROY) {
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, message, wparam, lparam);;
	}
	const Engine* instance = Engine::instance.value();

	switch(message) {
	case WM_CREATE:

	case WM_MOUSEMOVE:
		return WindowEvent::on_mouse_move(instance, hwnd, message, wparam, lparam);
	case WM_RBUTTONDOWN:
		return WindowEvent::on_mouse_right_button_down(instance, hwnd, message, wparam, lparam);
	case WM_RBUTTONUP:
		return WindowEvent::on_mouse_right_button_up(instance, hwnd, message, wparam, lparam);
	case WM_LBUTTONDOWN:
		return WindowEvent::on_mouse_left_button_down(instance, hwnd, message, wparam, lparam);
	case WM_LBUTTONUP:
		return WindowEvent::on_mouse_left_button_up(instance, hwnd, message, wparam, lparam);
	case WM_MOUSEHWHEEL:
		return WindowEvent::on_mouse_wheel(instance, hwnd, message, wparam, lparam);
	case WM_NCHITTEST:
		return WindowEvent::on_hit_test(instance, hwnd, message, wparam, lparam);
	case WM_ERASEBKGND:
		return TRUE; // DWMに任せる(背景のちらつき防止)
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}

void WindowEvent::hook_win_event(const HWINEVENTHOOK, const DWORD event, const HWND hwnd, const LONG idObject, const LONG, const DWORD, const DWORD) {
	if(event != EVENT_OBJECT_LOCATIONCHANGE) {
		return;
	}

	if(idObject != OBJID_WINDOW) {
		return;
	}

	if(hwnd != WindowEvent::target_hwnd) {
		return;
	}

	WindowEvent::snap_to_top_edge();
}