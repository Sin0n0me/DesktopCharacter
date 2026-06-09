#include "system_handle.h"

namespace enishi::types {
    bool SystemHandle::is_valid(void) const noexcept {
        return this->id != details::INVALID_HANDLE_ID;
    }
} // namespace enishi::types