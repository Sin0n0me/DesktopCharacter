#pragma once
#include "../errors/window_errors.h"
#include "../input/input.h"
#include <engine_types/window/window_types.h>
#include <foundation/result/result.h>
#include <optional>
#include <string>
#include "window_handle.h"

namespace enishi::platform {
    class IWindow {
      public:
        [[nodiscard]] virtual std::optional<WindowHandle> get_handle(
            void) const noexcept = 0;

        [[nodiscard]] virtual std::optional<const IInput*> get_input(void) const noexcept = 0;

        [[nodiscard]] virtual std::optional<types::WindowPosition> get_position(
            void) const noexcept = 0;

        [[nodiscard]] virtual foundation::VoidResult<WindowError> set_position(
            const types::WindowPosition& position) noexcept = 0;

        [[nodiscard]] virtual std::optional<types::WindowSize> get_size(void) const noexcept = 0;

        [[nodiscard]] virtual foundation::VoidResult<WindowError> set_size(
            const types::WindowSize& size) noexcept = 0;

        [[nodiscard]] virtual foundation::VoidResult<WindowError> set_title(
            const std::string& title) noexcept = 0;

        [[nodiscard]] virtual std::optional<std::string> get_title(void) const noexcept = 0;

        // プログラム側から閉じる場合
        virtual void close(void) = 0;

        virtual void poll_events(void) = 0;
    };
} // namespace enishi::platform