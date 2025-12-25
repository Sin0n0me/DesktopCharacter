#include <d3dcompiler.h>
#include <d3d11.h>
#include "ModelRenderPass.h"
#include "../../../Application.h"
#include "../CommonResource.h"

constexpr wchar_t VERTEX_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";
constexpr wchar_t PIXEL_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";

ModelRenderPass::ModelRenderPass(const std::shared_ptr<CommonResource>& common_resouce) : IRenderPass(common_resouce) {
	this->common_resouce = common_resouce;
}

bool ModelRenderPass::init(ID3D11Device* const device) {
	if(!this->make_shaders(device)) {
		return false;
	}

	if(!this->make_depth_stencil(device)) {
		return false;
	}

	if(!this->make_shadow_sampler(device)) {
		return false;
	}

	return true;
}

void ModelRenderPass::update(ID3D11DeviceContext* const context) {
}

void ModelRenderPass::render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const {
	context->OMSetRenderTargets(
		1,
		&render_target_view,
		this->depth_stencil_view.Get()
	);

	context->OMSetDepthStencilState(this->depth_stencil_state.Get(), 0);
}

void ModelRenderPass::render(ID3D11DeviceContext* const context) const {
	context->ClearDepthStencilView(
		this->depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	// シェーダーのバインド
	context->IASetInputLayout(this->model_input_layout.Get());
	context->VSSetShader(this->model_vertex_shader.Get(), nullptr, 0);
	context->PSSetShader(this->model_pixel_shader.Get(), nullptr, 0);

	// 定数バッファのバインド
	context->VSSetConstantBuffers(
		0,
		1,
		this->common_resouce->camera_constant_buffer.GetAddressOf()
	);
	context->VSSetConstantBuffers(
		2,
		1,
		this->common_resouce->shadow_constant_buffer.GetAddressOf()
	);

	// シャドウマップ
	context->PSSetShaderResources(1, 1, this->common_resouce->shadow_resouce_view.GetAddressOf());
	context->PSSetSamplers(1, 1, this->shadow_sampler.GetAddressOf());
}

bool ModelRenderPass::make_shaders(ID3D11Device* const device) {
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
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
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
			ps_blob.GetAddressOf(),
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
			this->model_vertex_shader.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		const HRESULT hr = device->CreatePixelShader(
			ps_blob->GetBufferPointer(),
			ps_blob->GetBufferSize(),
			nullptr,
			this->model_pixel_shader.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

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

	device->CreateInputLayout(
		layout,
		_countof(layout),
		vs_blob->GetBufferPointer(),
		vs_blob->GetBufferSize(),
		this->model_input_layout.GetAddressOf()
	);

	return true;
}

// 深度ステンシルの作成
bool ModelRenderPass::make_depth_stencil(ID3D11Device* const device) {
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = WIDTH;
		texDesc.Height = HEIGHT;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		const HRESULT hr = device->CreateTexture2D(
			&texDesc,
			nullptr,
			this->depth_texture.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		const HRESULT hr = device->CreateDepthStencilView(
			this->depth_texture.Get(),
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

		const HRESULT hr = device->CreateDepthStencilState(
			&desc,
			this->depth_stencil_state.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

// シャドウマップを使用するためのバッファ作成
bool ModelRenderPass::make_shadow_sampler(ID3D11Device* const device) {
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

		const HRESULT hr = device->CreateSamplerState(
			&desc,
			this->shadow_sampler.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}