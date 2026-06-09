#include "window_handle.h"

namespace enishi::platform {
    NativeWindowHandle WindowHandle::get_native_handle(void) const {
        return this->native_handle;
    }
} // namespace enishi::platform