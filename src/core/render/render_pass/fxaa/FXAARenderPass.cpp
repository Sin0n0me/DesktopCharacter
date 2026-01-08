#include "../../../../Application.h"
#include "../../../object/full_screen_quad/FullScreenQuad.h"
#include "../../CommonResource.h"
#include "../../constant_buffer/FXAA.h"
#include "FXAARenderPass.h"
#include <d3d11.h>
#include <d3dcompiler.h>

constexpr wchar_t PATH_PIXEL_SHADER_FXAA[] = L"assets/shader/ps_fxaa.hlsl";
constexpr wchar_t PATH_VERTEX_SHADER_FXAA[] = L"assets/shader/vs_fxaa.hlsl";

FXAARenderPass::FXAARenderPass(const std::shared_ptr<CommonResource>& common_resouce) :
	IRenderPass(common_resouce) {
	this->resource = common_resouce;
	this->quad_object = std::make_unique<FullScreenQuad>();
}

bool FXAARenderPass::make_shader(ID3D11Device* const device) {
	Microsoft::WRL::ComPtr<ID3DBlob> ps_blob;
	Microsoft::WRL::ComPtr<ID3DBlob> vs_blob;
	Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

	{
		const HRESULT hr = D3DCompileFromFile(
			PATH_VERTEX_SHADER_FXAA,
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
		const HRESULT hr = D3DCompileFromFile(
			PATH_PIXEL_SHADER_FXAA,
			nullptr,
			nullptr,
			"main",
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
			this->resource->vertex_shaders[Pattern::FXAA].GetAddressOf()
		);

		if(FAILED(hr)) {
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
			return false;
		}
	}

	{
		const HRESULT hr = device->CreatePixelShader(
			ps_blob->GetBufferPointer(),
			ps_blob->GetBufferSize(),
			nullptr,
			this->resource->pixel_shaders[Pattern::FXAA].GetAddressOf()
		);

		if(FAILED(hr)) {
			if(error_blob.Get()) {
				OutputDebugStringA((char*)error_blob->GetBufferPointer());
			}
			return false;
		}
	}

	{
		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TEXCOORD",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				8,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
		};

		const HRESULT hr = device->CreateInputLayout(
			layout,
			_countof(layout),
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			this->resource->input_layouts[Pattern::FXAA].GetAddressOf()
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

bool FXAARenderPass::init(ID3D11Device* const device) {
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(FXAA);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		FXAA fxaa{};
		fxaa.inverse_screen_size = {
			1.0f / WIDTH,
			1.0f / HEIGHT
		};
		fxaa.span_max = 8.0f;
		fxaa.reduce_min = 1.0f / 128.0f;
		fxaa.reduce_mul = 1.0f / 8.0f;

		D3D11_SUBRESOURCE_DATA init_data{};
		init_data.pSysMem = &fxaa;
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = sizeof(decltype(fxaa));

		const HRESULT hr = device->CreateBuffer(
			&desc,
			&init_data,
			this->resource->constant_buffers[ConstantBuffer::FXAA].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		const HRESULT hr = device->CreateSamplerState(
			&desc,
			this->resource->sampler_state[Pattern::FXAA].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = FALSE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = FALSE;

		const HRESULT hr = device->CreateDepthStencilState(
			&desc,
			this->resource->depth_stencil_state[Pattern::FXAA].GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	if(!this->make_shader(device)) {
		return false;
	}

	if(!this->quad_object->init(device)) {
		return false;
	}

	return true;
}

void FXAARenderPass::update(ID3D11DeviceContext* const context) {
}

void FXAARenderPass::render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const {
	context->OMSetRenderTargets(
		1,
		&render_target_view,
		nullptr
	);

	context->OMSetDepthStencilState(
		this->resource->depth_stencil_state.at(Pattern::FXAA).Get(),
		0
	);

	context->IASetInputLayout(
		this->resource->input_layouts.at(Pattern::FXAA).Get()
	);

	context->PSSetShader(
		this->resource->pixel_shaders.at(Pattern::FXAA).Get(),
		nullptr,
		0
	);
	context->VSSetShader(
		this->resource->vertex_shaders.at(Pattern::FXAA).Get(),
		nullptr,
		0
	);

	context->PSSetSamplers(
		0,
		1,
		this->resource->sampler_state.at(Pattern::FXAA).GetAddressOf()
	);
	context->PSSetConstantBuffers(
		0,
		1,
		this->resource->constant_buffers.at(ConstantBuffer::FXAA).GetAddressOf()
	);
}

void FXAARenderPass::render(ID3D11DeviceContext* const context) const {
	this->quad_object->render(context);
}

bool FXAARenderPass::is_render_model(void) const {
	return false;
}

bool FXAARenderPass::is_post_render(void) const {
	return true;
}