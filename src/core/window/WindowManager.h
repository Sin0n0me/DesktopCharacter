#pragma once
#include "WindowStatus.h"
#include <memory>

class IMouseStateGetter;
class Collider;
class IWindowFrame;
struct SDL_Window;
enum class GraphicsAPI;

class WindowManager {
private:
    const std::shared_ptr<IMouseStateGetter> mouse_state;
    std::unique_ptr<IWindowFrame> root_window;

    bool make_root_window(const GraphicsAPI& api);

public:

    explicit WindowManager(
        std::unique_ptr<IWindowFrame> root_window
    );
    ~WindowManager(void) noexcept;

    bool init(const GraphicsAPI& api);

    void update(const Collider* collider);

    WindowStatus get_window_status(void) const;

    std::optional<SDL_Window*> get_window(void) const;
};
