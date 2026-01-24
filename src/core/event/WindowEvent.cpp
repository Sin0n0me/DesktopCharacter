#include "../../Application.h"
#include "../log/Logger.h"
#include "WindowEvent.h"
#include <windowsx.h>

decltype(WindowEvent::cursor) WindowEvent::cursor = LoadCursor(nullptr, IDC_HAND);

LRESULT WindowEvent::on_input(const Engine* instance, const HWND hwnd, const WPARAM wparam, const LPARAM lparam) {
    UINT size = 0;
    GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lparam),
        RID_INPUT,
        nullptr,
        &size,
        sizeof(RAWINPUTHEADER)
    );
    std::vector<BYTE> buffer(size);

    if(GetRawInputData(
        reinterpret_cast<HRAWINPUT>(lparam),
        RID_INPUT,
        buffer.data(),
        &size,
        sizeof(RAWINPUTHEADER)
    ) != size) {
        return 0;
    }

    const RAWINPUT* raw = reinterpret_cast<const RAWINPUT*>(buffer.data());

    if(raw->header.dwType == RIM_TYPEMOUSE) {
        const RAWMOUSE& mouse = raw->data.mouse;
        WindowEvent::on_mouse_input(instance, mouse, hwnd, wparam, lparam);
    }

    return 0;
}

void WindowEvent::on_mouse_input(
    const Engine* instance,
    const RAWMOUSE& raw_mouse,
    const HWND hwnd,
    const WPARAM wparam,
    const LPARAM lparam
) {
    WindowEvent::on_mouse_move(instance, raw_mouse.lLastX, raw_mouse.lLastY, hwnd, wparam, lparam);

    const USHORT flags = raw_mouse.usButtonFlags;    // ボタン状態

    // 左ボタン押下
    if(flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
        WindowEvent::on_mouse_click_left(instance, false, hwnd, wparam, lparam);
    }
    // 左ボタン解放
    if(flags & RI_MOUSE_LEFT_BUTTON_UP) {
        WindowEvent::on_mouse_click_left(instance, true, hwnd, wparam, lparam);
    }

    if(flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
        // 右ボタン押下
    }
    if(flags & RI_MOUSE_RIGHT_BUTTON_UP) {
        // 右ボタン解放
    }

    if(flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        // 中ボタン押下
    }
    if(flags & RI_MOUSE_MIDDLE_BUTTON_UP) {
        // 中ボタン解放
    }

    // 垂直ホイール
    if(flags & RI_MOUSE_WHEEL) {
        const SHORT delta = static_cast<SHORT>(raw_mouse.usButtonData);
        // delta > 0 : 上方向
        // delta < 0 : 下方向
    }

    // 水平ホイール
    if(flags & RI_MOUSE_HWHEEL) {
        const SHORT delta = static_cast<SHORT>(raw_mouse.usButtonData);
        // delta > 0 : 右方向
        // del
    }
}

void WindowEvent::on_mouse_move(
    const Engine* instance,
    const LONG dx,
    const LONG dy,
    const HWND hwnd,
    const WPARAM wparam,
    const LPARAM lparam
) {
}

void WindowEvent::on_mouse_click_left(const Engine* instance, const bool is_up, const HWND hwnd, const WPARAM wparam, const LPARAM lparam) {
    if(is_up) {
        instance->mouse_state->is_dragging = false;
    } else {
        if(instance->collider->is_hit_model()) {
            instance->mouse_state->is_dragging = true;
        }
    }
}

LRESULT WindowEvent::wnd_proc(
    const HWND hwnd,
    const UINT message,
    const WPARAM wparam,
    const LPARAM lparam
) {
    if(!Engine::instance.has_value()) {
        if(message == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);;
    }
    const Engine* instance = Engine::instance.value();

    switch(message) {
    case WM_INPUT:
        return WindowEvent::on_input(instance, hwnd, wparam, lparam);
    case WM_ERASEBKGND:
        return TRUE; // DWMに任せる(背景のちらつき防止)
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::input_wnd_proc(
    const HWND hwnd,
    const UINT message,
    const WPARAM wparam,
    const LPARAM lparam
) {
    if(!Engine::instance.has_value()) {
        if(message == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);;
    }
    const Engine* instance = Engine::instance.value();

    switch(message) {
    case WM_INPUT:
        return WindowEvent::on_input(instance, hwnd, wparam, lparam);
    case WM_ERASEBKGND:
        return TRUE; // DWMに任せる
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_LBUTTONDOWN:
        break;
    case WM_LBUTTONUP:
        break;
    default:
        break;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}