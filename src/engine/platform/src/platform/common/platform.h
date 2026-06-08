#pragma once
#include <cstdint>

namespace platform {
    enum class Platform : std::uint32_t {
        Win32,
        Win64,
        X11,
        Wayland,
    };
}