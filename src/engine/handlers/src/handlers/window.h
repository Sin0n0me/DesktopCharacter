#pragma once
#include "../common/window_types.h"
#include "../window_system/window_handle.h"
#include <string>

namespace enishi::handler  {
        class IWindowFrame {
          public:
            [[nodiscard]] virtual WindowHandle get_handle(void) const = 0;

            [[nodiscard]] virtual WindowPosition get_position(void) const = 0;

            virtual void set_position(const WindowPosition position) = 0;

            [[nodiscard]] virtual WindowSize get_size(void) const = 0;

            virtual void set_size(const WindowSize size) = 0;

            virtual void set_title(const std::string& title) = 0;

            [[nodiscard]] virtual std::string get_title(void) const = 0;

            // プログラム側から閉じる場合
            virtual void close(void) = 0;
        };
} // namespace platform