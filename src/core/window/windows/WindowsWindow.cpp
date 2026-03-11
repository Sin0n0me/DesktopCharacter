#include "../../collider/Collider.h"
#include "../../log/Logger.h"
#include "WindowsWindow.h"
#include "WindowsWindowTranslator.h"
#include <SDL3/SDL.h>

constexpr LONG HIT_WINDOW_SIZE = 8;

bool set_layered_window(
    const HWND hwnd,
    const std::int32_t width,
    const std::int32_t height,
    const std::uint32_t color_abgr
) {
    const BITMAPINFO bitmap_info{
        .bmiHeader = {
            .biSize = sizeof(BITMAPINFOHEADER),
            .biWidth = width,
            .biHeight = -height, // top-down
            .biPlanes = 1,
            .biBitCount = 32,
            .biCompression = BI_RGB,
    }
    };

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
        return false;
    }

    // ビットマップの初期化
    {
        uint32_t* const pixels = static_cast<uint32_t*>(dib_bits);
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < width; ++x) {
                const int offset = y * width + x;
                pixels[offset] = color_abgr;
            }
        }
    }

    const HDC screen_dc = GetDC(nullptr);
    const HDC memory_dc = CreateCompatibleDC(screen_dc);
    if(!memory_dc) {
        ReleaseDC(nullptr, screen_dc);
        DeleteObject(dib_bitmap);
        return false;
    }

    const HBITMAP old_bitmap = static_cast<HBITMAP>(
        SelectObject(memory_dc, dib_bitmap)
        );

    SIZE window_size{width, height};
    POINT src_pos{0, 0};
    POINT dst_pos{0, 0};

    BLENDFUNCTION blend_function{
        AC_SRC_OVER,
        0,
        255,
        AC_SRC_ALPHA
    };

    const BOOL result = UpdateLayeredWindow(
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

    return result == TRUE;
}

WindowsWindow::WindowsWindow(void) {
}

bool WindowsWindow::create(SDLWindowPtr ptr) {
    if(!bool(ptr)) {
        return false;
    }

    this->window = std::move(ptr);
    const HWND hwnd = WindowsWindowTranslator::get_hwnd(this->window.get());
    if(hwnd == NULL) {
        Logger::error(u8"ハンドルの取得に失敗しました");
        return false;
    }

    // WS_EX_NOREDIRECTIONBITMAP で描画はGPUに任せる
    // WS_EX_LAYERED を指定してヒットテスト用のビットマップだけ保持してもらう
    // WS_EX_TRANSPARENT | WS_EX_NOACTIVATE
    const LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(
        hwnd,
        GWL_EXSTYLE,
        style | WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED
    );

    int width = 0;
    int height = 0;
    if(!SDL_GetWindowSize(
        this->window.get(),
        &width,
        &height
    )) {
        return false;
    }

    if(!set_layered_window(
        hwnd,
        width,
        height,
        0x00'00'00'00
    )) {
        return false;
    }

    if(!SDL_SetWindowPosition(
        this->window.get(),
        100,
        100
    )) {
        return false;
    }

    return true;
}

void WindowsWindow::on_update(const Collider* collider) {
    if(!collider->is_hit_model()) {
        return;
    }

    /*

    // モデル描画ウィンドウの移動
    // ドラッグしていればウィンドウごと移動
    if(this->mouse_state->get_is_dragging() && this->mouse_state->is_moved()) {
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
    */
}

WindowPoint WindowsWindow::get_window_size(void) const {
    int width = 0;
    int height = 0;
    if(!SDL_GetWindowSize(
        this->window.get(),
        &width,
        &height
    )) {
        Logger::warning(u8"ウィンドウサイズの取得に失敗しました");
    }

    return WindowPoint{
        .x = width,
        .y = height,
    };
}

WindowPoint WindowsWindow::get_screen_position(void) const {
    int width = 0;
    int height = 0;
    if(!SDL_GetWindowPosition(
        this->window.get(),
        &width,
        &height
    )) {
        Logger::warning(u8"ウィンドウ座標の取得に失敗しました");
    }

    return WindowPoint{
        .x = width,
        .y = height,
    };
}

std::optional<SDL_Window*> WindowsWindow::get_window(void) const {
    if(!bool(this->window)) {
        return std::nullopt;
    }

    return this->window.get();
}