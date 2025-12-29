#include "Application.h"
#include "core/Core.h"
#include "core/event/WindowEvent.h"

constexpr LPCWSTR WINDOW_NAME = IS_DEBUG_MODE ? L"ゆかりさんを自由に動かしたい(DebugMode)" : L"ゆかりさんを自由に動かしたい";

int WINAPI WinMain(const HINSTANCE hInst, const HINSTANCE, const LPSTR, const int) {
	WNDCLASS wc{};
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpfnWndProc = WindowEvent::wnd_proc;
	wc.hInstance = hInst;
	wc.lpszClassName = L"D3D11AndDXGI";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	// WS_EX_NOREDIRECTIONBITMAP だけだとカーソル先が透明部分の場合に処理が移譲されない
	const HWND hwnd = CreateWindowEx(
		WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED,
		wc.lpszClassName,
		WINDOW_NAME,
		WS_POPUPWINDOW,
		200,
		200,
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		hInst,
		nullptr
	);

	if(hwnd == NULL) {
		return -1;
	}

	// 常に最前面にする
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(hwnd, SW_SHOW);

	Engine engine = Engine();

	if(!engine.init(hwnd, WIDTH, HEIGHT)) {
		return -1;
	}

	engine.run();

	engine.uninit();

	return 0;
}