#pragma once
#include <memory>
#include <optional>
#include <Windows.h> // TODO削除

#ifdef _DEBUG
constexpr bool IS_DEBUG_MODE = true;
#else
constexpr bool IS_DEBUG_MODE = false;
#endif // _DEBUG

struct D3D11;
struct CommonResource;
struct MouseState;
class RenderPipeline;
class WindowManager;
class ModelManager;
class Scene;
class Collider;
class IMouseStateGetter;
class DeltaTime;

class Engine {
private:
    friend class WindowEvent; // 分離用

private:
    static std::optional<Engine* const> instance;

private:
    std::shared_ptr<D3D11> d3d11;
    std::shared_ptr<CommonResource> common_resource;
    std::unique_ptr<RenderPipeline> render_pipeline;
    std::unique_ptr<WindowManager> window_manager;

    std::shared_ptr<ModelManager> models;
    std::unique_ptr<Scene> scene;
    std::shared_ptr<MouseState> mouse_state;
    std::unique_ptr<Collider> collider;

private:

    void update(const DeltaTime& delta_time);
    void render_update(void);
    void render(void) const;

public:
    explicit Engine(
        const HINSTANCE& hinstance,
        const LPWSTR& cmd_line
    ) noexcept;
    ~Engine(void);

    //static Engine get_engine();

    const IMouseStateGetter* get_mouse_getter(void) const;

    bool init(const UINT& width, const UINT& height);
    void run(void);
    void uninit(void);
};
