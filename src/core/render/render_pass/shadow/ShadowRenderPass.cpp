#include "../../CommonResource.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../shader/Shader.h"
#include "../../shader/shadow/ShadowMapPixelShader.h"
#include "../../shader/shadow/ShadowMapVertexShader.h"
#include "ShadowRenderPass.h"
#include <d3d11.h>

constexpr wchar_t PATH_VERTEX_SHADER_SHADOW[] = L"assets/shader/vs_shadow.hlsl";
constexpr UINT SHADOW_MAP_SIZE = 2048;

ShadowRenderPass::ShadowRenderPass(const std::shared_ptr<CommonResource>& common_resouce) noexcept : RenderPass(common_resouce) {
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

void ShadowRenderPass::render_set(
    ID3D11DeviceContext* const context,
    ID3D11RenderTargetView* const render_target_view
) const {
    context->OMSetRenderTargets(
        0,
        nullptr,
        this->resource->depth_stencil_view.at(Pattern::ShadowMap).Get()
    );
    context->OMSetDepthStencilState(
        this->resource->depth_stencil_state.at(Pattern::ShadowMap).Get(),
        0
    );

    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = static_cast<FLOAT>(SHADOW_MAP_SIZE);
    vp.Height = static_cast<FLOAT>(SHADOW_MAP_SIZE);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    context->RSSetViewports(1, &vp);

    context->ClearDepthStencilView(
        this->resource->depth_stencil_view.at(Pattern::ShadowMap).Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    context->IASetInputLayout(
        this->resource->input_layouts.at(Pattern::ShadowMap).Get()
    );

    // シェーダーのバインド
    context->VSSetShader(
        this->resource->vertex_shaders.at(Pattern::ShadowMap).Get(),
        nullptr,
        0
    );
    context->PSSetShader(nullptr, nullptr, 0);

    // 定数バッファのバインド
    context->VSSetConstantBuffers(
        this->binding_slots->get(
            ShaderType::Vertex,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::Camera)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::Camera).GetAddressOf()
    );
    context->VSSetConstantBuffers(
        this->binding_slots->get(
            ShaderType::Vertex,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::ShadowMap)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::ShadowMap).GetAddressOf()
    );
}

void ShadowRenderPass::render(ID3D11DeviceContext* const context) const {
}

bool ShadowRenderPass::make_shaders(ID3D11Device* const device) {
    Shader vertex_shader = Shader(std::make_unique<ShadowMapVertexShader>());

    if(!vertex_shader.make_shader(
        device,
        this->resource->vertex_shaders[Pattern::ShadowMap].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_input_layout(
        device,
        this->resource->input_layouts[Pattern::ShadowMap].GetAddressOf()
    )) {
        return false;
    }

    /*
    Shader pixel_shader = Shader(std::make_unique<ShadowMapPixelShader>());
    if(!pixel_shader.make_shader(
        device,
        this->resource->pixel_shaders[Pattern::ShadowMap].GetAddressOf()
    )) {
        return false;
    }
    */

    // slot番号の取得
    this->binding_slots->merge(vertex_shader);

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
            this->resource->depth_stencil_view[Pattern::ShadowMap].GetAddressOf()
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
            this->resource->shader_resouce_view[Pattern::ShadowMap].GetAddressOf()
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
            this->resource->depth_stencil_state[Pattern::ShadowMap].GetAddressOf()
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
            this->resource->sampler_state[Pattern::ShadowMap].GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    return true;
}

void ShadowRenderPass::back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const {
}

bool ShadowRenderPass::is_post_render(void) const {
    return false;
}

bool ShadowRenderPass::is_render_model(void) const {
    return true;
}