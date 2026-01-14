#include "../shader/ShaderBindingSlots.h"
#include "Texture.h"
#include <d3d11.h>
#include <filesystem>
#include <vector>
#include <wincodec.h>

Texture::Texture(
    const uint32_t texture_name_hash,
    const uint32_t sampler_name_hash
) noexcept :
    texture_name_hash(texture_name_hash),
    sampler_name_hash(sampler_name_hash) {
}

bool Texture::load_texure(ID3D11Device* const device, const std::filesystem::path path) {
    Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
    {
        const HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(factory.GetAddressOf())
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    {
        const HRESULT hr = factory->CreateDecoderFromFilename(
            path.wstring().c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &decoder
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    {
        const HRESULT hr = decoder->GetFrame(0, &frame);
        if(FAILED(hr)) {
            return false;
        }
    }

    Microsoft::WRL::ComPtr<IWICFormatConverter> conv;
    {
        const HRESULT hr = factory->CreateFormatConverter(&conv);
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        const HRESULT hr = conv->Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            nullptr, 0.0,
            WICBitmapPaletteTypeCustom
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    {
        UINT width;
        UINT height;
        {
            const HRESULT hr = conv->GetSize(&width, &height);
            if(FAILED(hr)) {
                return false;
            }
        }

        std::vector<uint8_t> pixels(width * height * 4);
        {
            const HRESULT hr = conv->CopyPixels(
                nullptr, width * 4,
                pixels.size(),
                pixels.data()
            );
            if(FAILED(hr)) {
                return false;
            }
        }

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = pixels.data();
        data.SysMemPitch = width * 4;

        if(!this->make_texture(
            device,
            texture.GetAddressOf(),
            width,
            height,
            data
        )) {
            return false;
        }

        if(!this->make_sampler(device)) {
            return false;
        }
    }

    {
        const HRESULT hr = device->CreateShaderResourceView(
            texture.Get(),
            nullptr,
            this->shader_resouce_view.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    return true;
}

bool Texture::make_dummy_texture(ID3D11Device* const device) {
    const std::uint32_t white_pixel = 0xffffffff; // RGBA = 1,1,1,1
    const D3D11_TEXTURE2D_DESC texture_desc{
        .Width = 1,
        .Height = 1,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = {1, 0},
        .Usage = D3D11_USAGE_IMMUTABLE,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = &white_pixel,
        .SysMemPitch = sizeof(std::uint32_t),
        .SysMemSlicePitch = 0,
    };

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    {
        const HRESULT hr = device->CreateTexture2D(
            &texture_desc,
            &init_data,
            texture.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        const HRESULT hr = device->CreateShaderResourceView(
            texture.Get(),
            nullptr,
            this->shader_resouce_view.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        const D3D11_SAMPLER_DESC sampler_desc{
            .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
            .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
            .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
            .MipLODBias = 0.0f,
            .MaxAnisotropy = 1,
            .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
            .BorderColor = {0, 0, 0, 0},
            .MinLOD = 0.0f,
            .MaxLOD = D3D11_FLOAT32_MAX,
        };

        const HRESULT hr = device->CreateSamplerState(
            &sampler_desc,
            this->sampler.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    return true;
}

bool Texture::make_texture(
    ID3D11Device* const device,
    ID3D11Texture2D** texture,
    const UINT width,
    const  UINT height,
    const D3D11_SUBRESOURCE_DATA& data
) {
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    const HRESULT hr = device->CreateTexture2D(&desc, &data, texture);
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool Texture::make_sampler(ID3D11Device* const device) {
    D3D11_SAMPLER_DESC desc{};
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 1;
    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.BorderColor[0] = 0;
    desc.BorderColor[1] = 0;
    desc.BorderColor[2] = 0;
    desc.BorderColor[3] = 0;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    const HRESULT hr = device->CreateSamplerState(
        &desc,
        this->sampler.ReleaseAndGetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

void Texture::render_set_resource(
    ID3D11DeviceContext* const context,
    const ShaderBindingSlots* slots
) const {
    const BindingSlotKey texture_key = BindingSlotKey{
        this->texture_name_hash,
        ShaderType::Pixel,
        BindingSlotKind::Texture
    };
    if(slots->contains(texture_key)) {
        context->PSSetShaderResources(
            slots->get(texture_key),
            1,
            this->shader_resouce_view.GetAddressOf()
        );
    }

    const BindingSlotKey sampler_key = BindingSlotKey{
        this->sampler_name_hash,
        ShaderType::Pixel,
        BindingSlotKind::SamplerState
    };
    if(slots->contains(sampler_key)) {
        context->PSSetSamplers(
            slots->get(sampler_key),
            1,
            this->sampler.GetAddressOf()
        );
    }
}