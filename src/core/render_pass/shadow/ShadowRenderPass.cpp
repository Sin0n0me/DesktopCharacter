#include "ShadowRenderPass.h"
#include <d3dcompiler.h>
#include "../CommonResource.h"

constexpr wchar_t PATH_VERTEX_SHADER_SHADOW[] = L"assets/shader/vs_shadow.hlsl";

ShadowRenderPass::ShadowRenderPass(const std::shared_ptr<CommonResource>& common_resouce) : IRenderPass(common_resouce) {
	this->common_resouce = common_resouce;
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
	context->OMSetRenderTargets(
		0,
		nullptr,
		this->shadow_stencil_view.Get()
	);
}

void ShadowRenderPass::render(ID3D11DeviceContext* const context) const {
	context->ClearDepthStencilView(
		this->shadow_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	// シェーダーのバインド
	context->IASetInputLayout(this->shadow_input_layout.Get());
	context->VSSetShader(this->shadow_vertex_shader.Get(), nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

	// 定数バッファのバインド
	context->VSSetConstantBuffers(
		0,
		1,
		this->common_resouce->shadow_constant_buffer.GetAddressOf()
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
			this->shadow_vertex_shader.GetAddressOf()
		);

		if(FAILED(hr)) {
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
			return false;
		}
	}

	{
		const D3D11_INPUT_ELEMENT_DESC shadowLayout[] =
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
			shadowLayout,
			_countof(shadowLayout),
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			this->shadow_input_layout.GetAddressOf()
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
		desc.Width = 1024;
		desc.Height = 1024;
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
			this->shadow_stencil_view.GetAddressOf()
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
			this->common_resouce->shadow_resouce_view.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}