#pragma once
#include "../handler_types.h"
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

    constexpr static RenderHandle DEFAULT_RENDER_TARGET = RenderHandle{
        .id = details::INVALID_HANDLE_ID,
        .type = RenderHandleType::RenderTarget,
    };
} // namespace enishi::types
