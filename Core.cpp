#include <iostream>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "Core.h"
#include "Models.h"
#include "ConstantBuffers.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")

constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
constexpr wchar_t VERTEX_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";
constexpr wchar_t PIXEL_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";

//
// https://learn.microsoft.com/ja-jp/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
//
// DirectX11を使用するための構造体
struct D3D11Context {
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
	Microsoft::WRL::ComPtr<IDCompositionDevice> dcomp_device;
	Microsoft::WRL::ComPtr<IDCompositionTarget> target;
	Microsoft::WRL::ComPtr<IDCompositionVisual> visual;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_cull_back;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_cull_none;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	bool init_d3d11(const HWND hwnd, const UINT width, const  UINT height) {
		if(!this->make_device()) {
			return false;
		}

		if(!this->make_factory()) {
			return false;
		}

		if(!this->make_swap_chain(width, height)) {
			return false;
		}

		if(!this->make_render_target_view()) {
			return false;
		}

		if(!this->make_target(hwnd)) {
			return false;
		}

		if(!this->make_rasterizer()) {
			return false;
		}

		if(!this->make_visual()) {
			return false;
		}

		if(!this->commit()) {
			return false;
		}

		if(!this->make_shaders()) {
			return false;
		}

		if(!this->make_depth_stencil(width, height)) {
			return false;
		}

		this->set_viewport(width, height);

		return true;
	}

	// デバイスの作成
	bool make_device() {
		const HRESULT result_device = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr, 0,
			D3D11_SDK_VERSION,
			this->device.GetAddressOf(),
			nullptr,
			this->context.GetAddressOf()
		);
		if(FAILED(result_device)) {
			std::cerr << "Failed make D3D11Device" << std::endl;
			return false;
		}

		const HRESULT result_dxgi_device = this->device.As(&dxgi_device);
		if(FAILED(result_device)) {
			std::cerr << "Failed make DXGIDevice" << std::endl;
			return false;
		}

		const HRESULT result_dcom_device = DCompositionCreateDevice(
			dxgi_device.Get(),
			__uuidof(this->dcomp_device),
			reinterpret_cast<void**>(this->dcomp_device.GetAddressOf())
		);
		if(FAILED(result_device)) {
			std::cerr << "Failed make DCompositionDevice" << std::endl;
			return false;
		}

		return true;
	}

	// DXGIデバイスとFactory取得
	bool make_factory() {
		const HRESULT result_factory = CreateDXGIFactory2(
			DXGI_CREATE_FACTORY_DEBUG,
			__uuidof(this->factory),
			reinterpret_cast<void**>(this->factory.GetAddressOf())
		);
		if(FAILED(result_factory)) {
			std::cerr << "Failed make Factory" << std::endl;
			return false;
		}

		return true;
	}

	// スワップチェーンの作成
	bool make_swap_chain(const UINT width, const UINT height) {
		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = width;
		description.Height = height;
		description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		description.BufferCount = 2;
		description.SampleDesc.Count = 1;
		description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		description.Flags = IS_DEBUG_MODE ? D3D11_CREATE_DEVICE_DEBUG : 0;

		const HRESULT result_swap_chain = this->factory->CreateSwapChainForComposition(
			this->dxgi_device.Get(),
			&description,
			nullptr,
			this->swap_chain.GetAddressOf()
		);

		if(FAILED(result_swap_chain)) {
			std::cerr << "Failed make SwapChain" << std::endl;
			return false;
		}

		return true;
	}

	// RenderTargetView作成
	bool make_render_target_view() {
		// バックバッファ取得
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		this->swap_chain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

		const HRESULT hr = this->device->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			this->render_target_view.ReleaseAndGetAddressOf()
		);
		if(FAILED(hr)) {
			std::cerr << "Failed make RenderTargetView" << std::endl;
			return false;
		}

		return true;
	}

	bool make_target(const HWND hwnd) {
		const HRESULT hr = this->dcomp_device->CreateTargetForHwnd(
			hwnd,
			true, // Top most
			this->target.GetAddressOf()
		);

		if(FAILED(hr)) {
			std::cerr << "Failed make Target" << std::endl;
			return false;
		}

		return true;
	}

	bool make_visual() {
		const HRESULT hr = this->dcomp_device->CreateVisual(this->visual.GetAddressOf());
		if(FAILED(hr)) {
			std::cerr << "Failed make Visual" << std::endl;
			return false;
		}

		return true;
	}

	bool commit() {
		const HRESULT result_set_context = this->visual->SetContent(this->swap_chain.Get());
		if(FAILED(result_set_context)) {
			std::cerr << "Failed Set Context" << std::endl;
			return false;
		}

		const HRESULT result_set_root = this->target->SetRoot(this->visual.Get());
		if(FAILED(result_set_root)) {
			std::cerr << "Failed Set Root" << std::endl;
			return false;
		}

		// 合成エンジンに完了を通知
		const HRESULT result_commit = this->dcomp_device->Commit();
		if(FAILED(result_commit)) {
			std::cerr << "Failed Set Root" << std::endl;
			return false;
		}

		return true;
	}

	void create_blend_state(D3D11Context* const context, ID3D11BlendState* blend) {
		D3D11_BLEND_DESC bd{};
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		context->device->CreateBlendState(&bd, &blend);
	}

	// シェーダー作成
	bool make_shaders() {
		Microsoft::WRL::ComPtr<ID3DBlob> vs_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> ps_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

		{
			const HRESULT hr = D3DCompileFromFile(
				VERTEX_SHADER_PATH,
				nullptr,
				nullptr,
				"VSMain",
				"vs_5_0",
				0, 0,
				vs_blob.GetAddressOf(),
				error_blob.GetAddressOf()
			);

			if(FAILED(hr)) {
				std::cerr << "Failed Set Vertex Shader" << std::endl;
				return false;
			}
		}

		{
			const HRESULT hr = D3DCompileFromFile(
				PIXEL_SHADER_PATH,
				nullptr,
				nullptr,
				"PSMain",
				"ps_5_0",
				0, 0,
				&ps_blob,
				&error_blob
			);
			if(FAILED(hr)) {
				std::cerr << "Failed Set Pixel Shader" << std::endl;
				return false;
			}
		}

		this->device->CreateVertexShader(
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			nullptr,
			this->vertex_shader.GetAddressOf()
		);

		this->device->CreatePixelShader(
			ps_blob->GetBufferPointer(),
			ps_blob->GetBufferSize(),
			nullptr,
			this->pixel_shader.GetAddressOf()
		);

		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				12,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TEXCOORD",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				24,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};

		this->device->CreateInputLayout(
			layout,
			_countof(layout),
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			this->input_layout.GetAddressOf()
		);

		return true;
	}

	bool make_rasterizer() {
		D3D11_RASTERIZER_DESC desc{};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = TRUE;
		desc.FrontCounterClockwise = FALSE;

		// 背面カリングあり
		desc.CullMode = D3D11_CULL_BACK;
		if(FAILED(this->device->CreateRasterizerState(&desc, this->rasterizer_cull_back.GetAddressOf()))) {
			return false;
		}

		// カリングなし(両面)
		desc.CullMode = D3D11_CULL_NONE;
		if(FAILED(this->device->CreateRasterizerState(&desc, this->rasterizer_cull_none.GetAddressOf()))) {
			return false;
		}

		return true;
	}

	bool make_depth_stencil(const UINT width, const UINT height) {
		{
			D3D11_TEXTURE2D_DESC texDesc{};
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			texDesc.SampleDesc.Count = 1;
			texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			const HRESULT hr = this->device->CreateTexture2D(
				&texDesc,
				nullptr,
				this->depth_buffer.GetAddressOf()
			);
			if(FAILED(hr)) {
				return false;
			}
		}

		{
			const HRESULT hr = this->device->CreateDepthStencilView(
				this->depth_buffer.Get(),
				nullptr,
				this->depth_stencil_view.GetAddressOf()
			);
			if(FAILED(hr)) {
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			const HRESULT hr = this->device->CreateDepthStencilState(
				&desc,
				this->depth_stencil_state.GetAddressOf()
			);
			if(FAILED(hr)) {
				return false;
			}
		}

		return true;
	}

	void set_viewport(const UINT width, const UINT height) {
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = static_cast<FLOAT>(width);
		vp.Height = static_cast<FLOAT>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		this->context->RSSetViewports(1, &vp);
	}

	void set_render_targets() {
		this->context->ClearRenderTargetView(
			this->render_target_view.Get(),
			CLEAR_COLOR
		);
		this->context->ClearDepthStencilView(
			this->depth_stencil_view.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0
		);
		this->context->OMSetRenderTargets(
			1,
			this->render_target_view.GetAddressOf(),
			this->depth_stencil_view.Get()
		);
	}

	void set_state() {
		this->context->RSSetState(this->rasterizer_cull_back.Get());
		this->context->OMSetDepthStencilState(this->depth_stencil_state.Get(), 0);
	}

	void set_shaders() {
		this->context->IASetInputLayout(this->input_layout.Get());
		this->context->VSSetShader(this->vertex_shader.Get(), nullptr, 0);
		this->context->PSSetShader(this->pixel_shader.Get(), nullptr, 0);
	}

	void draw() {
		this->swap_chain->Present(1, 0);
	}
};

// main loop
int run_app(const HWND hwnd, const UINT width, const  UINT height) {
	D3D11Context context{};
	if(!context.init_d3d11(hwnd, width, height)) {
		return -1;
	}

	Microsoft::WRL::ComPtr<ID3D11BlendState> blend;
	context.create_blend_state(&context, blend.Get());

	// モデル読み込み
	auto loaded_models = Models::load_models();
	if(!loaded_models.has_value()) {
		return -2;
	}

	auto& models = loaded_models.value();
	models.set_current_model(context.device.Get());

	// 定数バッファ
	ConstantBuffers constant_buffers = ConstantBuffers::make_buffer(context.device.Get(), width, height);

	MSG msg{};
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		{
			constant_buffers.update_buffers(context.context.Get());
			models.update(context.context.Get());
		}

		{
			context.set_render_targets();
			context.set_state();
			constant_buffers.render_set_buffers(context.context.Get());
			context.set_shaders();
			models.render(context.context.Get());
			context.draw();
		}
	}

	return 0;
}