#include "../Application.h"
#include "Core.h"
#include <chrono>
#include "D3D11.h"
#include "render/CommonResource.h"
#include "render/render_pipeline/RenderPipeline.h"

decltype(Engine::instance) Engine::instance;

Engine::Engine(void) noexcept {
	this->common_resouce = std::make_shared<CommonResource>();
	this->d3d11 = std::make_shared<D3D11>();
	this->mouse_state = std::make_unique<MouseState>();
	this->collider = std::make_unique<Collider>();
}

void Engine::update(const int64_t delta_time) {
	this->models->update(delta_time);
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

bool Engine::init(const HWND& hwnd, const UINT& width, const UINT& height) {
	if(this->instance.has_value()) {
		return true;
	}

	const HRESULT hr = CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED);
	if(FAILED(hr)) {
		return false;
	}

	// DirectX11の初期化
	if(!this->d3d11->init_d3d11(hwnd, width, height)) {
		return false;
	}

	this->render_pipeline = std::make_unique<RenderPipeline>(this->d3d11, this->common_resouce);
	this->models = std::make_unique<ModelManager>(this->common_resouce);
	this->scene = std::make_unique<Scene>(this->d3d11->device.Get(), this->common_resouce);

	// パイプラインの初期化
	if(!this->render_pipeline->init()) {
		return false;
	}

	// モデル読み込み
	if(!this->models->init()) {
		return false;
	}
	if(!this->models->load_current_model(this->d3d11->device.Get())) {
		return false;
	}
	this->render_pipeline->on_model_changed(this->models->get_current_model());

	// シーンの作成
	if(!this->scene->init(this->d3d11->device.Get())) {
		return false;
	}

	this->instance.emplace(this);

	return true;
}

void Engine::run(void) {
	// main loop
	MSG msg{};
	auto start = std::chrono::high_resolution_clock::now();
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const auto end = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		this->update(duration.count());
		this->render_update();
		this->render();

		start = end;
	}
}

void Engine::uninit(void) {
	this->instance.reset();
	CoUninitialize();
}