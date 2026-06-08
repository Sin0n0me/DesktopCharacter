#pragma once
#include "../../interface/window.h"

namespace platform {
    namespace window {
        class WindowsWindow : public IWindowFrame {
          private:
            WindowHandle handle;
            WindowPosition position;
            WindowSize size;

          public:
            WindowHandle get_handle(void) const override;
            WindowPosition get_position(void) const override;
            void set_position(const WindowPosition position) override;
            WindowSize get_size(void) const override;
            void set_size(const WindowSize size) override;
            void set_title(const std::string& title) override;
            std::string get_title(void) const override;
            void close(void) override;
        };
    } // namespace window
} // namespace platform