#pragma once
#include "../common.h"
#include <cstdint>

namespace enishi::types {
    enum class RenderHandleType : std::uint32_t {
        Mesh,
        Texture,
        Shader,
        Buffer,
        RenderTarget,
    };

    struct RenderHandle {
        HandleId id;
        RenderHandleType type;

        bool is_valid(void) const noexcept;
    };
} // namespace enishi::types
