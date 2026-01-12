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
        nullptr
    );
    /*
    context->OMSetBlendState(
        this->blend_state.Get(),
        BLEND_COLOR,
        0xffffffff
    );
    */

    context->OMSetDepthStencilState(
        this->resource->depth_stencil_state.at(Pattern::ClearWall).Get(),
        0
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
    /*
    context->VSSetConstantBuffers(
        this->binding_slots->get(
            ShaderType::Vertex,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::ShadowMap)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::ShadowMap).GetAddressOf()
    );
    */

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
    /*
    */
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