#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <dcomp.h>
#include <iostream>
#include "D3D11.h"
#include "Core.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")

//
// https://learn.microsoft.com/ja-jp/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
//

bool D3D11::init_d3d11(const HWND hwnd, const UINT width, const UINT height) {
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

	this->set_viewport(width, height);

	return true;
}

// デバイスの作成
bool D3D11::make_device() {
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
bool D3D11::make_factory() {
	const HRESULT result_factory = CreateDXGIFactory2(
		DXGI_CREATE_FACTORY_DEBUG,
		__uuidof(this->dxgi_factory),
		reinterpret_cast<void**>(this->dxgi_factory.GetAddressOf())
	);
	if(FAILED(result_factory)) {
		std::cerr << "Failed make Factory" << std::endl;
		return false;
	}

	return true;
}

// スワップチェーンの作成
bool D3D11::make_swap_chain(const UINT width, const UINT height) {
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

	const HRESULT result_swap_chain = this->dxgi_factory->CreateSwapChainForComposition(
		this->dxgi_device.Get(),
		&description,
		nullptr,
		this->dxgi_swap_chain.GetAddressOf()
	);

	if(FAILED(result_swap_chain)) {
		std::cerr << "Failed make SwapChain" << std::endl;
		return false;
	}

	return true;
}

// RenderTargetView作成
bool D3D11::make_render_target_view() {
	// バックバッファ取得
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	this->dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

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

// ラスタライザの作成
bool D3D11::make_rasterizer() {
	D3D11_RASTERIZER_DESC desc{};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.DepthClipEnable = TRUE;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 1;
	desc.SlopeScaledDepthBias = 0.5f;

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

bool D3D11::make_target(const HWND hwnd) {
	const HRESULT hr = this->dcomp_device->CreateTargetForHwnd(
		hwnd,
		true, // Top most
		this->dcomp_target.GetAddressOf()
	);

	if(FAILED(hr)) {
		std::cerr << "Failed make Target" << std::endl;
		return false;
	}

	return true;
}

bool D3D11::make_visual() {
	const HRESULT hr = this->dcomp_device->CreateVisual(this->dcomp_visual.GetAddressOf());
	if(FAILED(hr)) {
		std::cerr << "Failed make Visual" << std::endl;
		return false;
	}

	return true;
}

bool D3D11::commit() {
	const HRESULT result_set_context = this->dcomp_visual->SetContent(this->dxgi_swap_chain.Get());
	if(FAILED(result_set_context)) {
		std::cerr << "Failed Set Context" << std::endl;
		return false;
	}

	const HRESULT result_set_root = this->dcomp_target->SetRoot(this->dcomp_visual.Get());
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

void D3D11::set_viewport(const UINT width, const UINT height) {
	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	this->context->RSSetViewports(1, &vp);;
}