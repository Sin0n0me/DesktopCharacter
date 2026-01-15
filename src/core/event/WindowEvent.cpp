#include "../../Application.h"
#include "WindowEvent.h"

void forward_message_to_underlying_window(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
    // 下のウィンドウのクライアント座標へ変換
    POINT pt{};
    pt.x = LOWORD(lparam);
    pt.y = HIWORD(lparam);

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
    if(!below || below == hwnd) {
        return;
    }

    ClientToScreen(hwnd, &pt);
    ScreenToClient(below, &pt);

    const LPARAM new_lparam = MAKELPARAM(pt.x, pt.y);
    SendMessage(below, message, wparam, new_lparam);
}

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

        // 相対移動量
        LONG dx = mouse.lLastX;
        LONG dy = mouse.lLastY;

        // ボタン状態
        USHORT flags = mouse.usButtonFlags;

        // ホイール
        if(flags & RI_MOUSE_WHEEL) {
            SHORT wheel = static_cast<SHORT>(mouse.usButtonData);
            // wheel は WHEEL_DELTA 単位
        }

        // ここで入力を処理
    }

    return 0;
}

LRESULT WindowEvent::on_mouse_move(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
    POINT pt{};
    pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
    pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標

    if(!WindowEvent::hit_model(instance, pt)) {
        return HTNOWHERE;
        //return WindowEvent::forward_message_to_underlying_window(hwnd, message, wparam, lparam);
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
        return 0;
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
    if(WindowEvent::hit_model(instance, pt)) {
        return 0;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_mouse_wheel(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
    return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::on_hit_test(const Engine* instance, const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
    POINT pt{};
    pt.x = LOWORD(lparam);	// lParamの下位16ビットはマウスカーソルのx座標
    pt.y = HIWORD(lparam);	// lParamの上位16ビットはマウスカーソルのy座標
    ScreenToClient(hwnd, &pt);

    if(WindowEvent::hit_model(instance, pt)) {
        return HTCLIENT;
    }

    //forward_message_to_underlying_window(hwnd, message, wparam, lparam);

    //DefWindowProc(hwnd, message, wparam, lparam);
    return HTTRANSPARENT;
}

LRESULT WindowEvent::input_wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
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
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT WindowEvent::render_wnd_proc(const HWND hwnd, const UINT message, const WPARAM wparam, const LPARAM lparam) {
    if(!Engine::instance.has_value()) {
        if(message == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);;
    }
    const Engine* instance = Engine::instance.value();

    switch(message) {
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