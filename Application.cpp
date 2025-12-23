#include "Core.h"
#include <windows.h>

constexpr UINT WIDTH = 400;
constexpr UINT HEIGHT = 600;
constexpr BYTE WINDOW_ALPHA = IS_DEBUG_MODE ? 10 : 0;
constexpr LPCWSTR WINDOW_NAME = IS_DEBUG_MODE ? L"ゆかりさんを自由に動かしたい(DebugMode)" : L"ゆかりさんを自由に動かしたい";

LRESULT CALLBACK WndProc(const HWND hwnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
	switch(message) {
	case WM_NCHITTEST:
	{
		// マウス座標（スクリーン座標）
		POINT pt{};

		// クライアント座標へ変換
		ScreenToClient(hwnd, &pt);

		// 透過部分であればウィンドウの当たり判定をなくしその先のウィンドウへ
		if(true) { // テスト用
			return HTTRANSPARENT;
		}

		const LRESULT hit = DefWindowProc(hwnd, message, wParam, lParam);

		// クライアント領域ならタイトルバー扱いにする
		return hit == HTCLIENT ? HTCAPTION : hit;
	}
	case WM_ERASEBKGND:
		return 1; // DWMに任せる(背景のちらつき防止)
	case WM_SYSKEYDOWN:
		return wParam == VK_RETURN ? 0 : DefWindowProc(hwnd, message, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
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
	wc.lpszClassName = L"D3DOverlay";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	// WS_EX_NOREDIRECTIONBITMAPだとマウスが
	const HWND hwnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT,
		wc.lpszClassName,
		WINDOW_NAME,
		WS_POPUPWINDOW,
		800,
		200,
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

	const int result = run_app(hwnd, WIDTH, HEIGHT);

	CoUninitialize();

	return result;
}