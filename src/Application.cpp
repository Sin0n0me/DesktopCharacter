#include "Application.h"
#include "core/Core.h"
#include "core/event/WindowEvent.h"

constexpr LPCWSTR WINDOW_NAME = IS_DEBUG_MODE ? L"ゆかりさんを自由に動かしたい(DebugMode)" : L"ゆかりさんを自由に動かしたい";

void set_layered_window(const HWND hwnd) {
	// --- BITMAPINFO 定義 ---
	const BITMAPINFO bitmap_info = [] {
		BITMAPINFO bmi{};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = WIDTH;
		bmi.bmiHeader.biHeight = -(long)HEIGHT; // top-down
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		return bmi;
		}();

	// --- DIB 作成 ---
	void* dib_bits = nullptr;

	const HBITMAP dib_bitmap = CreateDIBSection(
		nullptr,
		&bitmap_info,
		DIB_RGB_COLORS,
		&dib_bits,
		nullptr,
		0
	);

	if(!dib_bitmap || !dib_bits) {
		return;
	}

	// --- ビットマップの初期化 ---
	{
		uint8_t* const pixels = static_cast<uint8_t*>(dib_bits);
		constexpr size_t pixel_stride = 4;

		for(int y = 0; y < HEIGHT; ++y) {
			for(int x = 0; x < WIDTH; ++x) {
				const size_t offset = (static_cast<size_t>(y) * WIDTH + x) * pixel_stride;

				pixels[offset + 0] = 0x0;
				pixels[offset + 1] = 0x0;
				pixels[offset + 2] = 0x0;
				pixels[offset + 3] = 0x0;
			}
		}
	}

	// --- DC 準備 ---
	const HDC screen_dc = GetDC(nullptr);
	const HDC memory_dc = CreateCompatibleDC(screen_dc);

	const HBITMAP old_bitmap =
		static_cast<HBITMAP>(SelectObject(memory_dc, dib_bitmap));

	// --- UpdateLayeredWindow ---
	SIZE window_size{WIDTH, HEIGHT};
	POINT src_pos{0, 0};
	POINT dst_pos{0, 0};

	BLENDFUNCTION blend_function{
		AC_SRC_OVER,
		0,
		255,
		AC_SRC_ALPHA
	};

	UpdateLayeredWindow(
		hwnd,
		screen_dc,
		&dst_pos,
		&window_size,
		memory_dc,
		&src_pos,
		0,
		&blend_function,
		ULW_ALPHA
	);

	// --- 後始末 ---
	SelectObject(memory_dc, old_bitmap);
	DeleteDC(memory_dc);
	ReleaseDC(nullptr, screen_dc);
	DeleteObject(dib_bitmap);
}

int WINAPI WinMain(const HINSTANCE hinstance, const HINSTANCE, const LPSTR, const int) {
	WNDCLASSEX render_wc{};
	render_wc.cbSize = sizeof(decltype(render_wc));
	render_wc.hCursor = LoadCursor(nullptr, IDC_HAND);
	render_wc.lpfnWndProc = WindowEvent::input_wnd_proc;
	render_wc.hInstance = hinstance;
	render_wc.lpszClassName = L"D3D11AndDXGI";
	render_wc.style = CS_DBLCLKS | CS_OWNDC;
	RegisterClassEx(&render_wc);

	// WS_EX_NOREDIRECTIONBITMAP で描画はGPUに任せる
	// WS_EX_LAYERED を指定してヒットテスト用のビットマップだけ保持してもらう
	// WS_EX_TRANSPARENT | WS_EX_NOACTIVATE
	const HWND render_hwnd = CreateWindowEx(
		WS_EX_NOREDIRECTIONBITMAP,
		render_wc.lpszClassName,
		WINDOW_NAME,
		WS_POPUPWINDOW | WS_VISIBLE,
		800,
		400,
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		hinstance,
		nullptr
	);

	if(render_hwnd == NULL) {
		return -1;
	}

	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	/*
	RAWINPUTDEVICE raw_input_device[2]{};
	// mouse
	raw_input_device[0].usUsagePage = 0x01;
	raw_input_device[0].usUsage = 0x02;
	raw_input_device[0].dwFlags = RIDEV_INPUTSINK;
	raw_input_device[0].hwndTarget = input_hwnd;
	// keyboard
	raw_input_device[1].usUsagePage = 0x01;
	raw_input_device[1].usUsage = 0x06;
	raw_input_device[1].dwFlags = RIDEV_INPUTSINK;
	raw_input_device[1].hwndTarget = input_hwnd;
	RegisterRawInputDevices(
		raw_input_device,
		std::size(raw_input_device),
		sizeof(RAWINPUTDEVICE)
	);
	*/

	//set_layered_window(render_hwnd);

	//SetLayeredWindowAttributes(input_hwnd, 0, 1, LWA_ALPHA);

	// 常に最前面にする
	SetWindowPos(render_hwnd, HWND_TOPMOST, 800, 400, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(render_hwnd, SW_SHOW);

	Engine engine = Engine();

	if(!engine.init(render_hwnd, WIDTH, HEIGHT)) {
		return -2;
	}

	engine.run();

	engine.uninit();

	return 0;
}