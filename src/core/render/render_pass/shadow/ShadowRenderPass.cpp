#include "../../CommonResource.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../shader/Shader.h"
#include "../../shader/shadow/ShadowMapVertexShader.h"
#include "ShadowRenderPass.h"
#include <d3d11.h>

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
    context->ClearDepthStencilView(
        this->resource->depth_stencil_view.at(Pattern::ShadowMap).Get(),
        D3D11_CLEAR_DEPTH,
        1.0f,
        0
    );

    context->OMSetRenderTargets(
        0,
        nullptr,
        this->resource->depth_stencil_view.at(Pattern::ShadowMap).Get()
    );
    context->OMSetDepthStencilState(
        this->resource->depth_stencil_state.at(Pattern::ShadowMap).Get(),
        0
    );

    constexpr D3D11_VIEWPORT vp{
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = static_cast<FLOAT>(SHADOW_MAP_SIZE),
        .Height = static_cast<FLOAT>(SHADOW_MAP_SIZE),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    context->RSSetViewports(1, &vp);

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

    // slot番号の取得
    this->binding_slots->merge(vertex_shader);

    return true;
}

bool ShadowRenderPass::make_shadow_map(ID3D11Device* const device) {
    {
        constexpr DXGI_SAMPLE_DESC sample = {
            .Count = 1
        };
        constexpr D3D11_TEXTURE2D_DESC desc{
            .Width = SHADOW_MAP_SIZE,
            .Height = SHADOW_MAP_SIZE,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R32_TYPELESS,
            .SampleDesc = sample,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
        };

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
        constexpr D3D11_DEPTH_STENCIL_VIEW_DESC desc{
            .Format = DXGI_FORMAT_D32_FLOAT,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
        };

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
        constexpr D3D11_TEX2D_SRV texture{
            .MipLevels = 1
        };
        constexpr D3D11_SHADER_RESOURCE_VIEW_DESC desc{
            .Format = DXGI_FORMAT_R32_FLOAT,
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D = texture,
        };

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
        constexpr D3D11_DEPTH_STENCIL_DESC desc{
            .DepthEnable = TRUE,
            .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D11_COMPARISON_LESS,
        };

        const HRESULT hr = device->CreateDepthStencilState(
            &desc,
            this->resource->depth_stencil_state[Pattern::ShadowMap].GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        constexpr D3D11_SAMPLER_DESC desc{
            .Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
            //.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
            .AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
            .AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
            .MipLODBias = 0,
            .MaxAnisotropy = 1,
            .ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL,
            .BorderColor = {1.0f, 1.0f, 1.0f, 1.0f},
            .MinLOD = 0.0f,
            .MaxLOD = D3D11_FLOAT32_MAX,
        };

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