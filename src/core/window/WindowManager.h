#pragma once

#include "WindowStatus.h"
#include <memory>
#include <Windows.h>

class IMouseStateGetter;
class Collider;

class WindowManager {
private:
    const HINSTANCE hinstance;
    const std::shared_ptr<IMouseStateGetter> mouse_state;
    WindowStatus root_window;
    WindowStatus hit_window;

    bool make_root_window(void);
    bool make_hit_window(void);
    bool make_console_window(void); // debug
    bool register_input_device(void);

    void update_root_window(void);
    void update_hit_window(void);

public:

    explicit WindowManager(
        const HINSTANCE& hinstance,
        const LPWSTR& cmd_line,
        std::shared_ptr<IMouseStateGetter> mouse_state
    );

    bool init(void);

    void update(const Collider* collider);

    const WindowStatus& get_root_window_status(void) const noexcept;

    const WindowStatus& get_hit_window_status(void) const noexcept;
};
