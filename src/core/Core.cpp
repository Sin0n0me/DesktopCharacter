#include "Core.h"
#include "render_pass/model/ModelRenderPass.h"
#include "render_pass/shadow/ShadowRenderPass.h"
#include <d3d11.h>
#include <dxgi1_2.h>

//
// https://learn.microsoft.com/ja-jp/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
//
// DirectX11を使用するための構造体
//
// TODO: 神構造体が爆誕しているので分離する
struct DeletingThisStruct {
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadow_receiver_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shadow_receiver_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11BlendState> shadow_receiver_blend_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> shadow_receiver_depth_stencil;
};

void Engine::render_update(void) {
	for(auto& render_pass : this->render_pass) {
		render_pass->update(this->d3d11->context.Get());
	}

	this->models->update(this->d3d11->context.Get());
}

void Engine::render(void) const {
	for(const auto& render_pass : this->render_pass) {
		render_pass->render_set(
			this->d3d11->context.Get(),
			this->d3d11->render_target_view.Get()
		);

		this->d3d11->context->RSSetState(this->d3d11->rasterizer_cull_back.Get());

		render_pass->render(this->d3d11->context.Get());

		this->models->render(this->d3d11->context.Get());
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
	if(!engine.models->load_models()) {
		return std::optional<Engine>();
	}
	engine.models->set_current_model(engine.d3d11->device.Get());

	//
	engine.scene = std::make_unique<Scene>(engine.d3d11->device.Get(), engine.common_resouce);

	//
	engine.render_pass.emplace_back(
		std::make_unique<ShadowRenderPass>(engine.common_resouce)
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