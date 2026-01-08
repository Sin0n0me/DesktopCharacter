#include "../../../object/wall/WallObject.h"
#include "../../CommonResource.h"
#include "WallRenderPass.h"
#include <d3d11.h>
#include <d3dcompiler.h>

constexpr wchar_t VERTEX_SHADER_PATH[] = L"assets/shader/clear_wall.hlsl";
constexpr wchar_t PIXEL_SHADER_PATH[] = L"assets/shader/clear_wall.hlsl";
constexpr float BLEND_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

WallRenderPass::WallRenderPass(const std::shared_ptr<CommonResource>& common_resource) : IRenderPass(common_resource) {
	this->resource = common_resource;
	this->wall_object = std::make_unique<WallObject>();
}

bool WallRenderPass::init(ID3D11Device* const device) {
	if(!this->make_blend_state(device)) {
		return false;
	}

	if(!this->make_depth_state(device)) {
		return false;
	}

	if(!this->make_shaders(device)) {
		return false;
	}

	if(!this->wall_object->init(device)) {
		return false;
	}

	return true;
}

void WallRenderPass::update(ID3D11DeviceContext* const context) {
}

void WallRenderPass::render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const {
	context->OMSetRenderTargets(
		1,
		&render_target_view,
		this->resource->depth_stencil_view.at(Pattern::Model).Get()
	);
	context->OMSetBlendState(
		this->blend_state.Get(),
		BLEND_COLOR,
		0xffffffff
	);
	context->OMSetDepthStencilState(
		this->resource->depth_stencil_state.at(Pattern::Wall).Get(),
		0
	);

	context->IASetInputLayout(this->resource->input_layouts.at(Pattern::Wall).Get());

	// 定数バッファのバインド
	context->VSSetConstantBuffers(
		0,
		1,
		this->resource->constant_buffers.at(ConstantBuffer::Camera).GetAddressOf()
	);
	context->VSSetConstantBuffers(
		2,
		1,
		this->resource->constant_buffers.at(ConstantBuffer::Shadow).GetAddressOf()
	);

	// シェーダーのバインド
	context->VSSetShader(
		this->resource->vertex_shaders.at(Pattern::Wall).Get(),
		nullptr,
		0
	);
	context->PSSetShader(
		this->resource->pixel_shaders.at(Pattern::Wall).Get(),
		nullptr,
		0
	);

	// シャドウマップ
	context->PSSetShaderResources(
		0,
		1,
		this->resource->shader_resouce_view.at(Pattern::Shadow).GetAddressOf()
	);
	context->PSSetSamplers(
		0,
		1,
		this->resource->sampler_state.at(Pattern::Shadow).GetAddressOf()
	);
}

void WallRenderPass::render(ID3D11DeviceContext* const context) const {
	this->wall_object->render(context);
}

bool WallRenderPass::is_render_model(void) const {
	return false;
}

bool WallRenderPass::make_shaders(ID3D11Device* const device) {
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
			this->resource->vertex_shaders[Pattern::Wall].GetAddressOf()
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
			this->resource->pixel_shaders[Pattern::Wall].GetAddressOf()
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
		};

		const HRESULT hr = device->CreateInputLayout(
			layout,
			_countof(layout),
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			this->resource->input_layouts[Pattern::Wall].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

bool WallRenderPass::make_blend_state(ID3D11Device* const device) {
	D3D11_BLEND_DESC desc{};
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	const HRESULT hr = device->CreateBlendState(
		&desc,
		this->blend_state.GetAddressOf()
	);
	if(FAILED(hr)) {
		return false;
	}

	return true;
}

bool WallRenderPass::make_depth_state(ID3D11Device* const device) {
	D3D11_DEPTH_STENCIL_DESC desc{};
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 書かない
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	const HRESULT hr = device->CreateDepthStencilState(
		&desc,
		this->resource->depth_stencil_state[Pattern::Wall].GetAddressOf()
	);
	if(FAILED(hr)) {
		return false;
	}

	return true;
}

bool WallRenderPass::is_post_render(void) const {
	return false;
}