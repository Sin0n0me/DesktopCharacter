#pragma once
#include "window_types.h"
#include <cstdint>

namespace platform {
    namespace input {
        struct ClientMousePosition {
            std::int32_t x;
            std::int32_t y;
        };

        struct ScreenMousePosition {
            std::int32_t x;
            std::int32_t y;
        };

        ScreenMousePosition to_screen(const ClientMousePosition& mouse_position,
            const WindowPosition& window_position) const noexcept {
            return ScreenMousePosition{
                .x = window_position.x + mouse_positionx,
                .y = window_position.y + mouse_positiony,
            };
        }

        ClientMousePosition to_client(const ScreenMousePosition& mouse_position,
            const WindowPosition& window_position) const noexcept {
            return ClientMousePosition{
                .x = window_position.x - mouse_position.x,
                .y = window_position.y - mouse_position.y,
            };
        }
    } // namespace input
} // namespace platform