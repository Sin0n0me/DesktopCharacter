#include <d3dcompiler.h>
#include <d3d11.h>
#include "ModelRenderPass.h"
#include "../../../Application.h"
#include "../CommonResource.h"

constexpr wchar_t VERTEX_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";
constexpr wchar_t PIXEL_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";

ModelRenderPass::ModelRenderPass(const std::shared_ptr<CommonResource>& common_resouce) : IRenderPass(common_resouce) {
	this->resource = common_resouce;
}

bool ModelRenderPass::init(ID3D11Device* const device) {
	if(!this->make_shaders(device)) {
		return false;
	}

	if(!this->make_depth_stencil(device)) {
		return false;
	}

	return true;
}

void ModelRenderPass::update(ID3D11DeviceContext* const context) {
}

void ModelRenderPass::render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const {
	context->ClearDepthStencilView(
		this->resource->depth_stencil_view.at(Pattern::ModelPattern).Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	context->OMSetRenderTargets(
		1,
		&render_target_view,
		this->resource->depth_stencil_view.at(Pattern::ModelPattern).Get()
	);

	context->OMSetDepthStencilState(
		this->resource->depth_stencil_state.at(Pattern::ModelPattern).Get(),
		0
	);
}

void ModelRenderPass::render(ID3D11DeviceContext* const context) const {
	// シェーダーのバインド
	context->IASetInputLayout(this->resource->input_layouts.at(Pattern::ModelPattern).Get());
	context->VSSetShader(
		this->resource->vertex_shaders.at(Pattern::ModelPattern).Get(),
		nullptr,
		0
	);
	context->PSSetShader(
		this->resource->pixel_shaders.at(Pattern::ModelPattern).Get(),
		nullptr,
		0
	);

	// 定数バッファのバインド
	context->VSSetConstantBuffers(
		0,
		1,
		this->resource->constant_buffers.at(ConstantBufferPattern::CameraBuffer).GetAddressOf()
	);
	context->VSSetConstantBuffers(
		2,
		1,
		this->resource->constant_buffers.at(ConstantBufferPattern::ShadowBuffer).GetAddressOf()
	);

	// シャドウマップ
	context->PSSetShaderResources(
		1,
		1,
		this->resource->shader_resouce_view.at(Pattern::ShadowPattern).GetAddressOf()
	);
	context->PSSetSamplers(
		1,
		1,
		this->resource->sampler_state.at(Pattern::ShadowPattern).GetAddressOf()
	);
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
			this->resource->vertex_shaders[Pattern::ModelPattern].GetAddressOf()
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
			this->resource->pixel_shaders[Pattern::ModelPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		const D3D11_INPUT_ELEMENT_DESC layout[] = {
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

		const HRESULT hr = device->CreateInputLayout(
			layout,
			_countof(layout),
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			this->resource->input_layouts[Pattern::ModelPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

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
			this->resource->depth_stencil_view[Pattern::ModelPattern].GetAddressOf()
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
			this->resource->depth_stencil_state[Pattern::ModelPattern].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

bool ModelRenderPass::is_render_model(void) const {
	return true;
}