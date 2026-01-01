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
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpfnWndProc = WindowEvent::wnd_proc;
	wc.hInstance = hinstance;
	wc.lpszClassName = L"D3D11AndDXGI";
	wc.style = CS_DBLCLKS | CS_OWNDC;
	RegisterClassEx(&wc);

	// WS_EX_NOREDIRECTIONBITMAP で描画はGPUに任せる
	// WS_EX_LAYERED を指定してヒットテスト用のビットマップだけ保持してもらう
	// WS_EX_TRANSPARENT | WS_EX_NOACTIVATE
	const HWND hwnd = CreateWindowEx(
		WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED,
		wc.lpszClassName,
		WINDOW_NAME,
		WS_POPUPWINDOW,
		400,
		400,
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		hinstance,
		nullptr
	);

	if(hwnd == NULL) {
		return -1;
	}

	// 常に最前面にする
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(hwnd, SW_SHOW);

	set_layered_window(hwnd);

	Engine engine = Engine();

	if(!engine.init(hwnd, WIDTH, HEIGHT)) {
		return -2;
	}

	engine.run();

	engine.uninit();

	return 0;
}