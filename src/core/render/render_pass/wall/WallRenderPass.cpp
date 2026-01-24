#include "../../../object/wall/WallObject.h"
#include "../../CommonResource.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../shader/clear_wall/ClearWallPixelShader.h"
#include "../../shader/clear_wall/ClearWallVertexShader.h"
#include "../../shader/SamplerStateNames.h"
#include "../../shader/Shader.h"
#include "../../texrure/TextureNames.h"
#include "WallRenderPass.h"
#include <d3d11.h>

constexpr float BLEND_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

WallRenderPass::WallRenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept :
    RenderPass(common_resource) {
    this->wall_object = std::make_unique<WallObject>();
}

bool WallRenderPass::init(
    ID3D11Device* const device,
    ID3D11RenderTargetView* const render_target_view
) {
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
    // モデルと同じ
    context->OMSetRenderTargets(
        1,
        &render_target_view,
        nullptr
    );

    context->OMSetDepthStencilState(
        this->resource->depth_stencil_state.at(Pattern::ClearWall).Get(),
        0
    );
    context->OMSetBlendState(
        this->blend_state.Get(),
        BLEND_COLOR,
        0xFFFFFFFF
    );

    context->IASetInputLayout(
        this->resource->input_layouts.at(Pattern::ClearWall).Get()
    );

    // シェーダーのバインド
    context->VSSetShader(
        this->resource->vertex_shaders.at(Pattern::ClearWall).Get(),
        nullptr,
        0
    );
    context->PSSetShader(
        this->resource->pixel_shaders.at(Pattern::ClearWall).Get(),
        nullptr,
        0
    );

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

    // シャドウマップ
    context->PSSetShaderResources(
        this->binding_slots->get(
            ShaderType::Pixel,
            BindingSlotKind::Texture,
            static_cast<uint32_t>(TextureName::ShadowMap)
        ),
        1,
        this->resource->shader_resouce_view.at(Pattern::ShadowMap).GetAddressOf()
    );
    context->PSSetSamplers(
        this->binding_slots->get(
            ShaderType::Pixel,
            BindingSlotKind::SamplerState,
            static_cast<uint32_t>(SamplerStateName::ShadowMap)
        ),
        1,
        this->resource->sampler_state.at(Pattern::ShadowMap).GetAddressOf()
    );
}

bool WallRenderPass::should_reset_state(void) const {
    return true;
}

void WallRenderPass::render(ID3D11DeviceContext* const context) const {
    this->wall_object->render(context, this->binding_slots.get());
}

bool WallRenderPass::is_render_model(void) const {
    return false;
}

bool WallRenderPass::make_shaders(ID3D11Device* const device) {
    Shader vertex_shader = Shader(std::make_unique<ClearWallVertexShader>());
    Shader pixel_shader = Shader(std::make_unique<ClearWallPixelShader>());

    if(!pixel_shader.make_shader(
        device,
        this->resource->pixel_shaders[Pattern::ClearWall].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_shader(
        device,
        this->resource->vertex_shaders[Pattern::ClearWall].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_input_layout(
        device,
        this->resource->input_layouts[Pattern::ClearWall].GetAddressOf()
    )) {
        return false;
    }

    // slot番号の取得
    this->binding_slots->merge(vertex_shader);
    this->binding_slots->merge(pixel_shader);

    return true;
}

bool WallRenderPass::make_blend_state(ID3D11Device* const device) {
    constexpr D3D11_RENDER_TARGET_BLEND_DESC blend_desc{
        .BlendEnable = TRUE,
        .SrcBlend = D3D11_BLEND_SRC_ALPHA,
        .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
        .BlendOp = D3D11_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D11_BLEND_ONE,
        .DestBlendAlpha = D3D11_BLEND_ZERO,
        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
    };
    constexpr D3D11_BLEND_DESC desc{
        .RenderTarget = {blend_desc}
    };

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
    constexpr D3D11_DEPTH_STENCIL_DESC desc{
        .DepthEnable = TRUE,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO, // 書かない
        .DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
    };

    const HRESULT hr = device->CreateDepthStencilState(
        &desc,
        this->resource->depth_stencil_state[Pattern::ClearWall].GetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

void WallRenderPass::back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const {
}

bool WallRenderPass::is_post_render(void) const {
    return false;
}