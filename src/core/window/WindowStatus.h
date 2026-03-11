#pragma once
#include "../utility/Point.h"
#include <cstdint>

using WindowPoint = Point2<std::int32_t>;

class WindowStatus {
private:
    WindowPoint screen_position;   // 左上座標
    WindowPoint window_size;       // ウィンドウサイズ

public:
    explicit WindowStatus(
        const WindowPoint& screen_position,
        const WindowPoint& window_size
    ) noexcept;

    WindowPoint get_screen_position(void) const noexcept;
    WindowPoint get_window_size(void) const noexcept;
    WindowPoint to_client_position(const WindowPoint& pt) const noexcept;
    WindowPoint to_screen_position(const WindowPoint& pt) const noexcept;

    void update_screen_position(const WindowPoint& pt);
    void update_window_size(const WindowPoint& pt);
};
