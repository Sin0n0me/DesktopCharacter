#pragma once
#include <windows.h>

struct WindowStatus {
    HWND hwnd;             // ハンドル
    POINT screen_position; // 左上座標
    POINT window_size;     // ウィンドウサイズ

    POINT to_client_position(const POINT& pt) const;
    POINT to_screen_position(const POINT& pt) const;
};
