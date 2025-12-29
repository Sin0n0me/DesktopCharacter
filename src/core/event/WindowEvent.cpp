#include "WindowEvent.h"
#include "../../Application.h"

bool WindowEvent::hit_model(const Engine* instance, const POINT& point) {
	const Ray ray = Ray::make_ray_from_screen(
		static_cast<float>(point.x),
		static_cast<float>(point.y),
		instance->scene->get_camera().camera
	);

	for(const auto& obb : instance->models->get_current_model()->get_obb()) {
		if(instance->collider->hit_model(ray, obb)) {
			return true;
		}
	}

	return false;
}

LRESULT WindowEvent::forward_message_to_underlying_window(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	// 一時的に透過
	SetWindowLong(
		hwnd,
		GWL_EXSTYLE,
		GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT
	);

	// 下のウィンドウのクライアント座標へ変換
	POINT pt{};
	pt.x = LOWORD(lparam);
	pt.y = HIWORD(lparam);
	const HWND below = WindowFromPoint(pt);
	if(!below || below == hwnd) {
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	ClientToScreen(hwnd, &pt);
	ScreenToClient(below, &pt);
	const LPARAM new_lparam = MAKELPARAM(pt.x, pt.y);

	SendMessage(below, message, wparam, new_lparam);

	// 元に戻す
	SetWindowLong(
		hwnd,
		GWL_EXSTYLE,
		GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT
	);

	/*
	const HWND below2 = WindowFromPoint(pt);
	LONG oldStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, oldStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	SetWindowLong(hwnd, GWL_EXSTYLE, oldStyle);
	*/

	return 0;
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
	return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	return DefWindowProc(hwnd, message, wparam, lparam);
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
		return 0;
	case WM_NCHITTEST:
		break;
	case WM_ERASEBKGND:
		return 1; // DWMに任せる(背景のちらつき防止)
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}