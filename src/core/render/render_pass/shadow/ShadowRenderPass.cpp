#include "../../CommonResource.h"
#include "ShadowRenderPass.h"
#include <d3d11.h>
#include <d3dcompiler.h>

constexpr wchar_t PATH_VERTEX_SHADER_SHADOW[] = L"assets/shader/vs_shadow.hlsl";
constexpr UINT SHADOW_MAP_SIZE = 2048;

ShadowRenderPass::ShadowRenderPass(const std::shared_ptr<CommonResource>& common_resouce) : IRenderPass(common_resouce) {
	this->resource = common_resouce;
}

bool ShadowRenderPass::init(ID3D11Device* const device) {
	if(!this->make_shaders(device)) {
		return false;
	}

	if(!this->make_shadow_map(device)) {
		return false;
	}

	return true;
}

void ShadowRenderPass::update(ID3D11DeviceContext* const context) {
}

void ShadowRenderPass::render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const {
	context->ClearDepthStencilView(
		this->resource->depth_stencil_view.at(Pattern::ShadowPattern).Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);
	context->OMSetRenderTargets(
		0,
		nullptr,
		this->resource->depth_stencil_view.at(Pattern::ShadowPattern).Get()
	);

	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<FLOAT>(SHADOW_MAP_SIZE);
	vp.Height = static_cast<FLOAT>(SHADOW_MAP_SIZE);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	context->RSSetViewports(1, &vp);
}

void ShadowRenderPass::render(ID3D11DeviceContext* const context) const {
	context->IASetInputLayout(this->resource->input_layouts.at(Pattern::ShadowPattern).Get());

	// シェーダーのバインド
	context->VSSetShader(this->resource->vertex_shaders.at(Pattern::ShadowPattern).Get(), nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

	// 定数バッファのバインド
	context->VSSetConstantBuffers(
		0,
		1,
		this->resource->constant_buffers.at(ConstantBufferPattern::CameraBuffer).GetAddressOf()
	);
	context->VSSetConstantBuffers(
		1,
		1,
		this->resource->constant_buffers.at(ConstantBufferPattern::ShadowBuffer).GetAddressOf()
	);
}

bool ShadowRenderPass::make_shaders(ID3D11Device* const device) {
	Microsoft::WRL::ComPtr<ID3DBlob> vs_blob;
	Microsoft::WRL::ComPtr<ID3DBlob> ps_blob;
	Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

	{
		const HRESULT hr = D3DCompileFromFile(
			PATH_VERTEX_SHADER_SHADOW,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			0,
			0,
			vs_blob.GetAddressOf(),
			error_blob.GetAddressOf()
		);

		if(FAILED(hr)) {
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
			return false;
		}
	}

	{
		const HRESULT hr = device->CreateVertexShader(
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			nullptr,
			this->resource->vertex_shaders[Pattern::ShadowPattern].GetAddressOf()
		);

		if(FAILED(hr)) {
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
			return false;
		}
	}

	{
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
			}
		};

		const HRESULT hr = device->CreateInputLayout(
			layout,
			_countof(layout),
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			this->resource->input_layouts[Pattern::ShadowPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
			return false;
		}
	}

	return true;
}

bool ShadowRenderPass::make_shadow_map(ID3D11Device* const device) {
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = SHADOW_MAP_SIZE;
		desc.Height = SHADOW_MAP_SIZE;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		const HRESULT hr = device->CreateTexture2D(
			&desc,
			nullptr,
			this->shadow_texture.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		const HRESULT hr = device->CreateDepthStencilView(
			this->shadow_texture.Get(),
			&desc,
			this->resource->depth_stencil_view[Pattern::ShadowPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		const HRESULT hr = device->CreateShaderResourceView(
			this->shadow_texture.Get(),
			&desc,
			this->resource->shader_resouce_view[Pattern::ShadowPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		D3D11_SAMPLER_DESC desc{};
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 1;

		const HRESULT hr = device->CreateSamplerState(
			&desc,
			this->resource->sampler_state[Pattern::ShadowPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

bool ShadowRenderPass::is_render_model(void) const {
	return true;
}