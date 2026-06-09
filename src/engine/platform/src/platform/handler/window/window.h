#pragma once
#include <engine_types/window/window_handle.h>
#include <engine_types/window/window_types.h>
#include <string>

namespace enishi::platform {
    class IWindow {
      public:
        [[nodiscard]] virtual types::WindowHandle get_handle(void) const = 0;

        [[nodiscard]] virtual types::WindowPosition get_position(void) const = 0;

        virtual void set_position(const types::WindowPosition position) = 0;

        [[nodiscard]] virtual types::WindowSize get_size(void) const = 0;

        virtual void set_size(const types::WindowSize size) = 0;

        virtual void set_title(const std::string& title) = 0;

        [[nodiscard]] virtual std::string get_title(void) const = 0;

        // プログラム側から閉じる場合
        virtual void close(void) = 0;

        virtual void poll_events(void) = 0;
    };
} // namespace enishi::platform