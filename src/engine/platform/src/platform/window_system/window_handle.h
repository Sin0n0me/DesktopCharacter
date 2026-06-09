#pragma once
#include <cstdint>

// OS別のネイティブハンドル定義
#if defined(WIN32)
#include <Windows.h>

namespace enishi::platform {
    struct NativeWindowHandle {
        HWND hwnd;
        HINSTANCE hinstance;
    };
} // namespace enishi::platform

#elif defined(USE_WAYLAND)

struct wl_display;
struct wl_surface;

namespace enishi::platform {
    struct NativeWindowHandle {
        wl_display* display;
        wl_surface* surface;
    };
} // namespace enishi::platform

#elif defined(USE_X11)
#include <X11/Xlib.h>

namespace enishi::platform {
    struct NativeWindowHandle {
        Display* display;
        ::Window window; // Xlib の Window は unsigned long
    };
} // namespace enishi::platform

#elif defined(__APPLE__)

#else
#error "Unsupported platform: define USE_WAYLAND or USE_X11 on Linux"
#endif // defined(WIN32)

namespace enishi::platform {
    // 外部向けのウィンドウハンドル
    class WindowHandle {
      private:
        NativeWindowHandle native_handle;

      public:
        explicit WindowHandle(const NativeWindowHandle native) noexcept
            : native_handle(native) {
        }

        // 取得はコピーで返す
        [[nodiscard]] NativeWindowHandle get_native_handle(void) const;
    };
} // namespace enishi::platform