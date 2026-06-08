#pragma once
#include "render_handle.h"
#include "../common.h"
#include <cstdint>

namespace enishi::types {
    bool enishi::types::RenderHandle::is_valid(void) const noexcept {
        return this->id != details::INVALID_HANDLE_ID;
    }
} // namespace enishi::types