#include "../Application.h"
#include "Core.h"
#include "render/render_pass/model/ModelRenderPass.h"
#include "render/render_pass/shadow/ShadowRenderPass.h"
#include "render/render_pass/wall/WallRenderPass.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <chrono>
#include "render/render_pass/fxaa/FXAARenderPass.h"

decltype(Engine::instance) Engine::instance;

void Engine::update(const int64_t delta_time) {
	this->models->update(delta_time);
}

void Engine::render_update(void) {
	for(auto& render_pass : this->render_pass) {
		render_pass->update(this->d3d11->context.Get());
	}

	this->scene->render_update(this->d3d11->context.Get());

	this->models->update_render(this->d3d11->context.Get());
}

void Engine::render(void) const {
	D3D11_VIEWPORT view_port{};
	view_port.TopLeftX = 0.0f;
	view_port.TopLeftY = 0.0f;
	view_port.Width = static_cast<FLOAT>(WIDTH);
	view_port.Height = static_cast<FLOAT>(HEIGHT);
	view_port.MinDepth = 0.0f;
	view_port.MaxDepth = 1.0f;

	this->d3d11->context->ClearRenderTargetView(
		this->d3d11->render_target_view.Get(),
		CLEAR_COLOR
	);
	this->d3d11->context->ClearRenderTargetView(
		this->d3d11->render_target_view_back.Get(),
		CLEAR_COLOR
	);

	for(const auto& render_pass : this->render_pass) {
		this->d3d11->context->RSSetViewports(1, &view_port);
		this->d3d11->context->RSSetState(this->d3d11->rasterizer_cull_back.Get());

		if(render_pass->is_post_render()) {
			render_pass->render_set(
				this->d3d11->context.Get(),
				this->d3d11->render_target_view_back.Get()
			);
			this->d3d11->context->PSSetShaderResources(
				0,
				1,
				this->d3d11->shader_resouce_view.GetAddressOf()
			);
		} else {
			render_pass->render_set(
				this->d3d11->context.Get(),
				this->d3d11->render_target_view.Get()
			);
		}

		render_pass->render(this->d3d11->context.Get());

		// モデルの描画が必要な場合
		if(render_pass->is_render_model()) {
			this->models->render(this->d3d11->context.Get());
		}

		ID3D11RenderTargetView* null_rtv[8] = {};
		this->d3d11->context->OMSetRenderTargets(8, null_rtv, nullptr);
		this->d3d11->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		this->d3d11->context->OMSetDepthStencilState(nullptr, 0);
		this->d3d11->context->IASetInputLayout(nullptr);
		this->d3d11->context->VSSetShader(nullptr, nullptr, 0);
		this->d3d11->context->PSSetShader(nullptr, nullptr, 0);
	}

	this->d3d11->dxgi_swap_chain->Present(1, 0);
}

const IMouseStateGetter* Engine::get_mouse_getter(void) const {
	return this->mouse_state.get();
}

bool Engine::init(const HWND& hwnd, const UINT& width, const UINT& height) {
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	const HRESULT hr = CoInitializeEx(nullptr, tagCOINIT::COINIT_APARTMENTTHREADED);
	if(FAILED(hr)) {
		return false;
	}

	// DirectX11の初期化
	this->d3d11 = std::make_unique<D3D11>();
	if(!this->d3d11->init_d3d11(hwnd, width, height)) {
		return false;
	}

	// 共通リソースの作成
	this->common_resouce = std::make_shared<CommonResource>();

	// マウスの状態を保存する構造体の作成
	this->mouse_state = std::make_unique<MouseState>();

	// 当たり判定用
	this->collider = std::make_unique<Collider>();

	// モデル読み込み
	this->models = std::make_unique<ModelManager>(this->common_resouce);
	if(!this->models->init()) {
		return false;
	}
	if(!this->models->load_current_model(this->d3d11->device.Get())) {
		return false;
	}

	// シーンの作成
	this->scene = std::make_unique<Scene>(this->d3d11->device.Get(), this->common_resouce);

	// レンダーパスの追加
	this->render_pass.emplace_back(
		std::make_unique<ShadowRenderPass>(this->common_resouce)
	);
	this->render_pass.emplace_back(
		std::make_unique<WallRenderPass>(this->common_resouce)
	);
	this->render_pass.emplace_back(
		std::make_unique<ModelRenderPass>(this->common_resouce)
	);
	this->render_pass.emplace_back(
		std::make_unique<FXAARenderPass>(this->common_resouce)
	);

	// レンダーパスの初期化
	for(auto& render_pass : this->render_pass) {
		if(!render_pass->init(this->d3d11->device.Get())) {
			return false;
		}
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