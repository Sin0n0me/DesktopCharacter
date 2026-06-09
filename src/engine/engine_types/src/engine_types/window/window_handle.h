#pragma once
#include "../handler_types.h"
#include <cstdint>

namespace enishi::types {
    enum class GuiBackend : std::uint32_t {
        Win32,
        Wayland,
        X11,
    };

    struct WindowHandle {
        HandleId id;
        GuiBackend type;

        bool is_valid(void) const noexcept;
    };
} // namespace enishi::types