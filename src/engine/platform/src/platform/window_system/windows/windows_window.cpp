#include "windows_window.h"

namespace platform {
    namespace window {
        WindowHandle WindowsWindow::get_handle(void) const {
            return this->handle;
        }

        WindowPosition WindowsWindow::get_position(void) const {
            return this->position;
        }

        void WindowsWindow::set_position(const WindowPosition position) {
            this->position = position;
        }

        WindowSize WindowsWindow::get_size(void) const {
            return this->size;
        }

        void WindowsWindow::set_size(const WindowSize size) {
            this->size = size;
        }

        void WindowsWindow::set_title(const std::string& title) {
        }

        std::string WindowsWindow::get_title(void) const {
            return std::string();
        }

        void WindowsWindow::close(void) {
        }
    } // namespace window
} // namespace platform