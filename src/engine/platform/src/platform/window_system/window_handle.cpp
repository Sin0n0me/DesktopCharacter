#include "window_handle.h"

namespace platform {
    namespace window {
        NativeWindowHandle WindowHandle::get_native_handle(void) const {
            return this->native_handle;
        }
    } // namespace window
} // namespace platform