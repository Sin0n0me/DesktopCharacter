#include "Core.h"
#include "render_pass/model/ModelRenderPass.h"
#include "render_pass/shadow/ShadowRenderPass.h"
#include "render_pass/wall/WallRenderPass.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include "../Application.h"

void Engine::render_update(void) {
	for(auto& render_pass : this->render_pass) {
		render_pass->update(this->d3d11->context.Get());
	}

	this->scene->render_update(this->d3d11->context.Get());

	this->models->update(this->d3d11->context.Get());
}

void Engine::render(void) const {
	D3D11_VIEWPORT view_port{};
	view_port.TopLeftX = 0.0f;
	view_port.TopLeftY = 0.0f;
	view_port.Width = static_cast<FLOAT>(WIDTH);
	view_port.Height = static_cast<FLOAT>(HEIGHT);
	view_port.MinDepth = 0.0f;
	view_port.MaxDepth = 1.0f;

	for(const auto& render_pass : this->render_pass) {
		this->d3d11->context->RSSetViewports(1, &view_port);
		this->d3d11->context->RSSetState(this->d3d11->rasterizer_cull_back.Get());

		render_pass->render_set(
			this->d3d11->context.Get(),
			this->d3d11->render_target_view.Get()
		);

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

std::optional<Engine> Engine::make_engine(const HWND hwnd, const UINT width, const UINT height) {
	Engine engine{};

	// DirectX11の初期化
	engine.d3d11 = std::make_unique<D3D11>();
	if(!engine.d3d11->init_d3d11(hwnd, width, height)) {
		return std::optional<Engine>();
	}

	// 共通リソースの作成
	engine.common_resouce = std::make_shared<CommonResource>();

	// モデル読み込み
	engine.models = std::make_unique<Models>();
	if(!engine.models->init()) {
		return std::optional<Engine>();
	}
	engine.models->load_current_model(engine.d3d11->device.Get());

	//
	engine.scene = std::make_unique<Scene>(engine.d3d11->device.Get(), engine.common_resouce);

	//
	engine.render_pass.emplace_back(
		std::make_unique<ShadowRenderPass>(engine.common_resouce)
	);
	engine.render_pass.emplace_back(
		std::make_unique<WallRenderPass>(engine.common_resouce)
	);
	engine.render_pass.emplace_back(
		std::make_unique<ModelRenderPass>(engine.common_resouce)
	);

	for(auto& render_pass : engine.render_pass) {
		if(!render_pass->init(engine.d3d11->device.Get())) {
			return std::optional<Engine>();
		}
	}

	return engine;
}

int Engine::run_app() {
	// main loop
	MSG msg{};
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		this->render_update();
		this->render();
	}

	return 0;
}