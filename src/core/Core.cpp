#include "../Application.h"
#include "Core.h"
#include "D3D11.h"
#include "log/Logger.h"
#include "render/CommonResource.h"
#include "render/render_pipeline/RenderPipeline.h"
#include "utility/Maker.h"

decltype(Engine::instance) Engine::instance;

Engine::Engine(const HINSTANCE& hinstance, const LPWSTR& cmd_line) noexcept {
    Maker::make_shared(this->common_resouce);
    Maker::make_shared(this->d3d11);
    Maker::make_shared(
        this->models,
        this->common_resouce
    );
    Maker::make_shared(this->mouse_state);
    Maker::make_unique(
        this->scene,
        this->common_resouce
    );
    Maker::make_unique(
        this->render_pipeline,
        this->d3d11,
        this->common_resouce
    );
    Maker::make_unique(
        this->collider,
        this->scene->get_camera(),
        this->models
    );
    Maker::make_unique(
        this->window_manager,
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
        ->get_root_window_status()
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
    if(this->instance.has_value()) {
        return true;
    }

    // このスレッドのCOMの初期化
    const HRESULT hr = CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED);
    if(FAILED(hr)) {
        Logger::error(u8"COMの初期化に失敗しました");
        return false;
    }

    // ウィンドウ管理クラスの初期化
    if(!this->window_manager->init()) {
        Logger::error(u8"ウィンドウ管理クラスの初期化に失敗しました");
        return false;
    }

    // DirectX11の初期化
    const auto& root = this->window_manager->get_root_window_status();
    if(!this->d3d11->init_d3d11(
        root.hwnd,
        root.window_size.x,
        root.window_size.y
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
    if(!this->mouse_state->init(root.hwnd)) {
        return false;
    }

    this->instance.emplace(this);

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

        this->render_update();
        this->update(duration);
        this->render();
    }
}

void Engine::uninit(void) {
    this->instance.reset();
    CoUninitialize();
}