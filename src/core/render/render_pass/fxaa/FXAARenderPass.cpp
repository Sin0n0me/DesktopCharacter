#include "../../../../Application.h"
#include "../../../object/full_screen_quad/FullScreenQuad.h"
#include "../../CommonResource.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../constant_buffer/FXAA.h"
#include "../../shader/fxaa/FXAAPixelShader.h"
#include "../../shader/fxaa/FXAAVertexShader.h"
#include "../../shader/SamplerStateNames.h"
#include "../../shader/Shader.h"
#include "../../texrure/TextureNames.h"
#include "FXAARenderPass.h"
#include <d3d11.h>

FXAARenderPass::FXAARenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept :
    RenderPass(common_resource) {
    this->quad_object = std::make_unique<FullScreenQuad>();
}

bool FXAARenderPass::make_shader(ID3D11Device* const device) {
    Shader vertex_shader = Shader(std::make_unique<FXAAVertexShader>());
    Shader pixel_shader = Shader(std::make_unique<FXAAPixelShader>());

    if(!vertex_shader.make_shader(
        device,
        this->resource->vertex_shaders[Pattern::FXAA].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_input_layout(
        device,
        this->resource->input_layouts[Pattern::FXAA].GetAddressOf()
    )) {
        return false;
    }

    if(!pixel_shader.make_shader(
        device,
        this->resource->pixel_shaders[Pattern::FXAA].GetAddressOf()
    )) {
        return false;
    }

    // slotの取得
    this->binding_slots->merge(vertex_shader);
    this->binding_slots->merge(pixel_shader);

    return true;
}

bool FXAARenderPass::make_constant_buffer(ID3D11Device* const device) {
    constexpr D3D11_BUFFER_DESC desc = {
        .ByteWidth = sizeof(FXAA),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    constexpr FXAA fxaa{
        .inverse_screen_size = {
            1.0f / WIDTH,
            1.0f / HEIGHT
    },
        .span_max = 8.0f,
        .reduce_min = 1.0f / 128.0f,
        .reduce_mul = 1.0f / 8.0f,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = &fxaa,
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(decltype(fxaa)),
    };

    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        this->resource->constant_buffers[ConstantBuffer::FXAA].GetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool FXAARenderPass::make_sampler(ID3D11Device* const device) {
    constexpr D3D11_SAMPLER_DESC desc{
        .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
        .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
        .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
        .ComparisonFunc = D3D11_COMPARISON_NEVER,
        .MinLOD = 0,
        .MaxLOD = D3D11_FLOAT32_MAX,
    };

    const HRESULT hr = device->CreateSamplerState(
        &desc,
        this->resource->sampler_state[Pattern::FXAA].GetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool FXAARenderPass::make_depth_stencil(ID3D11Device* const device) {
    constexpr D3D11_DEPTH_STENCIL_DESC desc{
        .DepthEnable = FALSE,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc = D3D11_COMPARISON_ALWAYS,
        .StencilEnable = FALSE,
    };

    const HRESULT hr = device->CreateDepthStencilState(
        &desc,
        this->resource->depth_stencil_state[Pattern::FXAA].GetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool FXAARenderPass::init(
    ID3D11Device* const device,
    ID3D11RenderTargetView* const render_target_view
) {
    if(!this->make_constant_buffer(device)) {
        return false;
    }

    if(!this->make_sampler(device)) {
        return false;
    }

    if(!this->make_depth_stencil(device)) {
        return false;
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

    context->PSSetConstantBuffers(
        this->binding_slots->get(
            ShaderType::Pixel,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::FXAA)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::FXAA).GetAddressOf()
    );
}

bool FXAARenderPass::should_reset_state(void) const {
    return true;
}

void FXAARenderPass::render(ID3D11DeviceContext* const context) const {
    this->quad_object->render(context, this->binding_slots.get());
}

bool FXAARenderPass::is_render_model(void) const {
    return false;
}

bool FXAARenderPass::is_post_render(void) const {
    return true;
}

void FXAARenderPass::back_buffer_resouce(
    ID3D11DeviceContext* const context,
    ID3D11ShaderResourceView* const shader_resouce_view
) const {
    context->PSSetShaderResources(
        this->binding_slots->get(
            ShaderType::Pixel,
            BindingSlotKind::Texture,
            static_cast<uint32_t>(TextureName::FXAA)
        ),
        1,
        &shader_resouce_view
    );
    context->PSSetSamplers(
        this->binding_slots->get(
            ShaderType::Pixel,
            BindingSlotKind::SamplerState,
            static_cast<uint32_t>(SamplerStateName::FXAA)
        ),
        1,
        this->resource->sampler_state.at(Pattern::FXAA).GetAddressOf()
    );
}

RasterizerKind FXAARenderPass::rasterizer_kind(void) const {
    return RasterizerKind::CullBack;
}