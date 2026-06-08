#pragma once
#include "../common.h"
#include <cstdint>

namespace enishi::types {
    enum class WindowHandleType : std::uint32_t {
        Mesh,
        Texture,
        Shader,
        Buffer,
        RenderTarget,
    };

    struct WindowHandle {
        HandleId id;
        WindowHandleType type;

        bool is_valid(void) const noexcept;
    };
} // namespace enishi::types