#include "../Application.h"
#include "../config/GraphicsAPI.h"
#include "collider/Collider.h"
#include "Core.h"
#include "D3D11.h"
#include "event/mouse/MouseState.h"
#include "log/Logger.h"
#include "render/CommonResource.h"
#include "render/model/ModelManager.h"
#include "render/render_pipeline/RenderPipeline.h"
#include "scene/Scene.h"
#include "timer/DeltaTime.h"
#include "timer/Timer.h"
#include "window/WindowManager.h"
#include "window/windows/WindowsWindowTranslator.h"

decltype(Engine::instance) Engine::instance;

Engine::Engine(const HINSTANCE& hinstance, const LPWSTR& cmd_line) noexcept :
    common_resource(std::make_shared<CommonResource>()),
    d3d11(std::make_shared<D3D11>()),
    mouse_state(std::make_shared<MouseState>()) {
    this->models = std::make_shared<ModelManager>(this->common_resource);
    this->scene = std::make_unique<Scene>(this->common_resource);
    this->render_pipeline = std::make_unique<RenderPipeline>(
        this->d3d11,
        this->common_resource
    );
    this->collider = std::make_unique<Collider>(
        this->scene->get_camera(),
        this->models
    );
    this->window_manager = std::make_unique<WindowManager>(
        hinstance,
        cmd_line,
        this->mouse_state
    );
}

Engine::~Engine(void) {
    this->uninit();
}

void Engine::update(const DeltaTime& delta_time) {
    this->mouse_state->update();
    this->window_manager->update(this->collider.get());
    this->models->update(delta_time);

    // 現在のマウス位置での当たり判定の更新
    const auto client_position = this->window_manager
        ->get_window_status()
        .to_client_position(
            this->mouse_state->mouse_position
        );
    this->collider->set_client_position(
        client_position.x,
        client_position.y
    );
}

void Engine::render_update(void) {
    this->render_pipeline->render_update();

    this->scene->render_update(this->d3d11->context.Get());

    this->models->update_render(this->d3d11->context.Get());
}

void Engine::render(void) const {
    this->render_pipeline->render();
}

const IMouseStateGetter* Engine::get_mouse_getter(void) const {
    return this->mouse_state.get();
}

bool Engine::init(const UINT& width, const UINT& height) {
    if(Engine::instance.has_value()) {
        return true;
    }

    // このスレッドのCOMの初期化
    const HRESULT hr = CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED);
    if(FAILED(hr)) {
        Logger::error(u8"COMの初期化に失敗しました");
        return false;
    }

    // ウィンドウ管理クラスの初期化
    if(!this->window_manager->init(GraphicsAPI::DirectX11)) {
        Logger::error(u8"ウィンドウ管理クラスの初期化に失敗しました");
        return false;
    }

    // DirectX11の初期化
    const auto& status = this->window_manager->get_window_status();
    const auto window_size = status.get_window_size();

    const auto opt_window = this->window_manager->get_window();
    if(!opt_window.has_value()) {
        Logger::error(u8"ハンドルの取得に失敗しました");
        return false;
    }
    const auto hwnd = WindowsWindowTranslator::get_hwnd(opt_window.value());

    if(!this->d3d11->init_d3d11(
        hwnd,
        window_size.x,
        window_size.y
    )) {
        Logger::error(u8"D3D11の初期化に失敗しました");
        return false;
    }

    // シーンの作成
    if(!this->scene->init(this->d3d11->device.Get())) {
        Logger::error(u8"シーンの初期化に失敗しました");
        return false;
    }

    // パイプラインの初期化
    if(!this->render_pipeline->init()) {
        Logger::error(u8"パイプラインの初期化に失敗しました");
        return false;
    }

    // モデル読み込み
    if(!this->models->init()) {
        Logger::error(u8"モデル管理の初期化に失敗しました");
        return false;
    }
    if(!this->models->load_current_model(this->d3d11->device.Get())) {
        return false;
    }
    this->render_pipeline->on_model_changed(this->models->get_current_model());

    // TODO: separate
    if(!this->mouse_state->init(hwnd)) {
        return false;
    }

    Engine::instance.emplace(this);

    Logger::info(u8"エンジンの初期化に成功しました");

    return true;
}

void Engine::run(void) {
    // main loop
    MSG msg{};
    Timer timer{};
    while(msg.message != WM_QUIT) {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        const auto duration = timer.get_delta_time();
        timer.frame_reset();

        this->update(duration);
        this->render_update();
        this->render();
    }
}

void Engine::uninit(void) {
    if(Engine::instance.has_value()) {
        Engine::instance.reset();
        CoUninitialize();
    }
}