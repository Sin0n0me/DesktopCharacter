#include "Texture.h"
#include <wincodec.h>
#include <d3d11.h>
#include <vector>
#include <filesystem>

void Texture::load_texure(ID3D11Device* const device, const std::filesystem::path path) {
	Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
	const HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&factory)
	);
	if(FAILED(hr)) {
		throw;
	}

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	factory->CreateDecoderFromFilename(
		path.wstring().c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	decoder->GetFrame(0, &frame);

	Microsoft::WRL::ComPtr<IWICFormatConverter> conv;
	factory->CreateFormatConverter(&conv);

	conv->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherTypeNone,
		nullptr, 0.0,
		WICBitmapPaletteTypeCustom
	);

	{
		UINT width;
		UINT height;
		conv->GetSize(&width, &height);

		std::vector<uint8_t> pixels(width * height * 4);
		conv->CopyPixels(nullptr, width * 4, pixels.size(), pixels.data());

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = pixels.data();
		data.SysMemPitch = width * 4;

		if(!this->make_texture(device, width, height, data)) {
			throw;
		}

		if(!this->make_sampler(device)) {
			throw;
		}
	}

	device->CreateShaderResourceView(
		this->texture.Get(),
		nullptr,
		this->shader_resouce_view.GetAddressOf()
	);
}

bool Texture::make_texture(ID3D11Device* const device, const UINT width, const  UINT height, const D3D11_SUBRESOURCE_DATA& data) {
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	auto hr = device->CreateTexture2D(&desc, &data, this->texture.GetAddressOf());
	if(FAILED(hr)) {
		return false;
	}

	return true;
}

bool Texture::make_sampler(ID3D11Device* const device) {
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&samplerDesc, this->sampler.ReleaseAndGetAddressOf());
	if(FAILED(hr)) {
		return false;
	}

	return true;
}

void Texture::render_set_resource(ID3D11DeviceContext* const context) const {
	context->PSSetShaderResources(0, 1, this->shader_resouce_view.GetAddressOf());
	context->PSSetSamplers(0, 1, this->sampler.GetAddressOf());
}