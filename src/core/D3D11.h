#pragma once

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include <dcomp.h>

// DirectX11を使用するための構造体
// 扱いやすさ重視で基本public(アプリケーションのコアとなる部分でしか作成しないため)
struct D3D11 {
public:
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain;
	Microsoft::WRL::ComPtr<IDCompositionDevice> dcomp_device;
	Microsoft::WRL::ComPtr<IDCompositionTarget> dcomp_target;
	Microsoft::WRL::ComPtr<IDCompositionVisual> dcomp_visual;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_back;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resouce_view; //
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_cull_back;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_cull_none;

public:
	D3D11(void) = default;
	bool init_d3d11(const HWND hwnd, const UINT width, const  UINT height);

private:

	bool make_device(void);
	bool make_factory(void);
	bool make_swap_chain(const UINT width, const UINT height);
	bool make_render_target_view(void);
	bool make_rasterizer(void);
	bool make_target(const HWND hwnd);
	bool make_visual(void);
	bool commit(void);
	void set_viewport(const UINT width, const UINT height);
};
