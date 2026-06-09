#pragma once
#include "window_handle.h"
#include <cstdint>

namespace enishi::types {
    bool enishi::types::WindowHandle::is_valid(void) const noexcept {
        return this->id != details::INVALID_HANDLE_ID;
    }
} // namespace enishi::types