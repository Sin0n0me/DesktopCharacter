#include "../../Application.h"
#include "../event/WindowEvent.h"
#include "../log/Logger.h"
#include "WindowManager.h"

constexpr LPCWSTR WINDOW_NAME = IS_DEBUG_MODE ? L"ゆかりさんを自由に動かしたい(DebugMode)" : L"ゆかりさんを自由に動かしたい";

void set_layered_window(const HWND hwnd) {
    constexpr BITMAPINFO bitmap_info = [] {
        constexpr BITMAPINFOHEADER bmi_haeader{
            .biSize = sizeof(BITMAPINFOHEADER),
            .biWidth = WIDTH,
            .biHeight = -(long)HEIGHT, // top-down
            .biPlanes = 1,
            .biBitCount = 32,
            .biCompression = BI_RGB,
        };
        constexpr BITMAPINFO bmi{
            .bmiHeader = bmi_haeader,
        };
        return bmi;
        }();

    // DIB 作成
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

    // ビットマップの初期化
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

    const HDC screen_dc = GetDC(nullptr);
    const HDC memory_dc = CreateCompatibleDC(screen_dc);
    const HBITMAP old_bitmap = static_cast<HBITMAP>(
        SelectObject(memory_dc, dib_bitmap)
        );

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

    // 後始末
    SelectObject(memory_dc, old_bitmap);
    DeleteDC(memory_dc);
    ReleaseDC(nullptr, screen_dc);
    DeleteObject(dib_bitmap);
}

WindowManager::WindowManager(
    const HINSTANCE& hinstance,
    const LPWSTR& cmd_line,
    std::shared_ptr<IMouseStateGetter> mouse_state
) :
    hinstance(hinstance),
    mouse_state(mouse_state) {
    // フォーマッタくんがいい感じの仕事をしてくれない...
    this->root_window = {
        .hwnd = NULL,
        .screen_position = {
            .x = 0,
            .y = 0,
    },
    .window_size{
            .x = WIDTH,
            .y = HEIGHT,
    },
    };

    this->hit_window = {
        .hwnd = NULL,
        .screen_position = {
            .x = 0,
            .y = 0,
    },
    .window_size{
            .x = 4,
            .y = 4,
    },
    };
}

bool WindowManager::make_root_window(void) {
    const WNDCLASSEX wc{
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_OWNDC | CS_NOCLOSE,
        .lpfnWndProc = WindowEvent::wnd_proc,
        .hInstance = this->hinstance,
        .hCursor = LoadCursor(nullptr, IDC_HAND),
        .lpszClassName = L"D3D11AndDXGI",
    };

    RegisterClassEx(&wc);

    // WS_EX_NOREDIRECTIONBITMAP で描画はGPUに任せる
    // WS_EX_LAYERED を指定してヒットテスト用のビットマップだけ保持してもらう
    // WS_EX_TRANSPARENT | WS_EX_NOACTIVATE
    this->root_window.hwnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED,
        wc.lpszClassName,
        WINDOW_NAME,
        WS_POPUPWINDOW | WS_VISIBLE,
        this->root_window.screen_position.x,
        this->root_window.screen_position.y,
        this->root_window.window_size.x,
        this->root_window.window_size.y,
        nullptr,
        nullptr,
        this->hinstance,
        nullptr
    );

    if(this->root_window.hwnd == NULL) {
        Logger::error(u8"親ウィンドウハンドルの生成に失敗しました");
        return false;
    }

    // 常に最前面にする
    const BOOL successed_set_window_pos = SetWindowPos(
        this->root_window.hwnd,
        HWND_TOPMOST,
        this->root_window.screen_position.x,
        this->root_window.screen_position.y,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE
    );
    if(successed_set_window_pos == FALSE) {
        return false;
    }

    set_layered_window(this->root_window.hwnd);

    return true;
}

bool WindowManager::make_hit_window(void) {
    const WNDCLASSEX wc{
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_OWNDC | CS_NOCLOSE,
        .lpfnWndProc = WindowEvent::input_wnd_proc,
        .hInstance = this->hinstance,
        .hCursor = LoadCursor(nullptr, IDC_HAND),
        .lpszClassName = L"InputOnlyChildWindow",
    };

    RegisterClassEx(&wc);

    this->hit_window.hwnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName,
        nullptr,
        WS_POPUPWINDOW | WS_VISIBLE,
        this->hit_window.screen_position.x,
        this->hit_window.screen_position.y,
        this->hit_window.window_size.x,
        this->hit_window.window_size.y,
        this->root_window.hwnd,
        nullptr,
        this->hinstance,
        nullptr
    );
    if(this->hit_window.hwnd == NULL) {
        Logger::error(u8"入力用ウィンドウハンドルの生成に失敗しました");
        return -1;
    }

    return true;
}

bool WindowManager::make_console_window(void) {
    if(!IS_DEBUG_MODE) {
        return true;
    }

    if(AllocConsole() == FALSE) {
        return false;
    }

    if(SetConsoleCP(CP_UTF8) == FALSE) {
        Logger::error(u8"UTF-8にセットできませんでした");
    }

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    return true;
}

bool WindowManager::register_input_device(void) {
    const RAWINPUTDEVICE raw_input_device[2]{
        // mouse
        {
            .usUsagePage = 0x01,
            .usUsage = 0x02,
            .dwFlags = RIDEV_INPUTSINK,
            .hwndTarget = this->root_window.hwnd,
        },
        // keyboard
        {
            .usUsagePage = 0x01,
            .usUsage = 0x06,
            .dwFlags = RIDEV_INPUTSINK,
            .hwndTarget = this->root_window.hwnd,
        }
    };

    const BOOL is_success = RegisterRawInputDevices(
        raw_input_device,
        std::size(raw_input_device),
        sizeof(RAWINPUTDEVICE)
    );

    return is_success == TRUE;
}

bool WindowManager::init(void) {
    if(!this->make_root_window()) {
        return false;
    }

    if(!this->make_hit_window()) {
        return false;
    }

    if(!this->make_console_window()) {
        return false;
    }

    if(!this->register_input_device()) {
        return false;
    }

    return true;
}

void WindowManager::update(const Collider* collider) {
    if(!collider->is_hit_model()) {
        return;
    }

    if(!this->mouse_state->is_moved()) {
        return;
    }

    // モデル描画ウィンドウの移動
    // ドラッグしていればウィンドウごと移動
    if(this->mouse_state->get_is_dragging()) {
        this->update_root_window();

        SetWindowPos(
            this->root_window.hwnd,
            HWND_TOPMOST,
            this->root_window.screen_position.x,
            this->root_window.screen_position.y,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW
        );
    }

    // マウス判定用ウィンドウの移動
    // モデルに触れている部分だけ移動
    this->update_hit_window();
    SetWindowPos(
        this->hit_window.hwnd,
        NULL,
        this->hit_window.screen_position.x,
        this->hit_window.screen_position.y,
        0,
        0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE
    );
}

void WindowManager::update_root_window(void) {
    const POINT pt = this->mouse_state->get_mouse_position();
    const POINT client_pre_pt = this->root_window.to_client_position(
        this->mouse_state->get_pre_mouse_position()
    );
    this->root_window.screen_position.x = pt.x - client_pre_pt.x;
    this->root_window.screen_position.y = pt.y - client_pre_pt.y;
}

void WindowManager::update_hit_window(void) {
    // 親ウィンドウより外に出ないように修正
    const POINT& pt = this->mouse_state->get_mouse_position();
    const POINT root_lt = this->root_window.screen_position;
    const POINT root_rb = {
        .x = root_lt.x + this->root_window.window_size.x,
        .y = root_lt.y + this->root_window.window_size.y,
    };
    const auto& size = this->hit_window.window_size;
    const int right_bottom_x = pt.x + size.x;
    const int right_bottom_y = pt.y + size.y;
    const int offset_x = size.x / 2;
    const int offset_y = size.x / 2;

    this->hit_window.screen_position.x = pt.x - offset_x;
    this->hit_window.screen_position.y = pt.y - offset_y;

    if(pt.x < root_lt.x) {
        this->hit_window.screen_position.x = root_lt.x - offset_x;
    }
    if(pt.y < root_lt.y) {
        this->hit_window.screen_position.y = root_lt.y - offset_y;
    }

    if(root_rb.x < right_bottom_x) {
        this->hit_window.screen_position.x = root_rb.x - size.x + offset_x;
    }
    if(root_rb.y < right_bottom_y) {
        this->hit_window.screen_position.y = root_rb.y - size.y + offset_y;
    }
}

const WindowStatus& WindowManager::get_root_window_status(void) const noexcept {
    return this->root_window;
}

const WindowStatus& WindowManager::get_hit_window_status(void) const noexcept {
    return this->hit_window;
}