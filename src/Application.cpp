#include "Application.h"
#include "core/Core.h"
#include <windows.h>

constexpr BYTE WINDOW_ALPHA = IS_DEBUG_MODE ? 10 : 0;
constexpr LPCWSTR WINDOW_NAME = IS_DEBUG_MODE ? L"ゆかりさんを自由に動かしたい(DebugMode)" : L"ゆかりさんを自由に動かしたい";

bool is_dragging = false;
int screen_position_x = 800;
int screen_position_y = 200;
POINT pre_mouse_position{};

void forward_mouse_message_to_underlying_window(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	// 一時的に透過
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);

	// 下のウィンドウのクライアント座標へ変換
	POINT pt{};
	pt.x = LOWORD(lparam);
	pt.y = HIWORD(lparam);
	const HWND below = WindowFromPoint(pt);
	if(!below || below == hwnd) {
		return;
	}
	ClientToScreen(hwnd, &pt);
	ScreenToClient(below, &pt);
	const LPARAM new_lparam = MAKELPARAM(pt.x, pt.y);

	SendMessage(below, message, wparam, new_lparam);

	// 元に戻す
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);

	/*
	const HWND below2 = WindowFromPoint(pt);
	LONG oldStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, oldStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	SetWindowLong(hwnd, GWL_EXSTYLE, oldStyle);
	*/
}

LRESULT CALLBACK WndProc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
	switch(message) {
	case WM_MOUSEMOVE:
	{
		POINT pt{};
		pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
		pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標

		if(is_dragging) {
			const int diff_x = pt.x - pre_mouse_position.x;
			const int diff_y = pt.y - pre_mouse_position.y;
			screen_position_x += diff_x;
			screen_position_y += diff_y;
			MoveWindow(hwnd, screen_position_x, screen_position_y, WIDTH, HEIGHT, FALSE);

			// ウィンドウ自体も移動しているため補正(移動先でまた移動した判定になるので荒ぶる)
			pt.x -= diff_x;
			pt.y -= diff_y;
		}

		pre_mouse_position = pt;

		break;
	}
	case WM_RBUTTONDOWN:
	{
		forward_mouse_message_to_underlying_window(hwnd, message, wparam, lparam);
		return 0;
	}
	case WM_LBUTTONDOWN:
		is_dragging = true;
		break;
	case WM_LBUTTONUP:
		is_dragging = false;
		break;
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

int WINAPI WinMain(const HINSTANCE hInst, HINSTANCE, LPSTR, int) {
	const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(FAILED(hr)) {
		throw;
	}

	WNDCLASS wc{};
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInst;
	wc.lpszClassName = L"D3D11AndDXGI";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	// WS_EX_NOREDIRECTIONBITMAP だとカーソル先が透明部分の場合に処理が移譲されない
	// WS_EX_LAYERED | WS_EX_TRANSPARENT あとDMWによる描画のコピーが発生して重たい
	const HWND hwnd = CreateWindowEx(
		WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED,
		wc.lpszClassName,
		WINDOW_NAME,
		WS_POPUPWINDOW,
		screen_position_x,
		screen_position_y,
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		hInst,
		nullptr
	);

	// 常に最前面にする
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(hwnd, SW_SHOW);

	auto has_engine = Engine::make_engine(hwnd, WIDTH, HEIGHT);
	if(!has_engine.has_value()) {
		return -1;
	}
	auto& engine = has_engine.value();

	const int result = engine.run_app();

	CoUninitialize();

	return result;
}