#include "../../../log/Logger.h"
#include "../../CommonResource.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../constant_buffer/ModelEdge.h"
#include "../../shader/model_edge/ModelEdgePixelShader.h"
#include "../../shader/model_edge/ModelEdgeVertexShader.h"
#include "../../shader/Shader.h"
#include "../../shader/ShaderBindingSlots.h"
#include "EdgeRenderPass.h"
#include <d3d11.h>

EdgeRenderPass::EdgeRenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept
    : RenderPass(common_resource) {
}

bool EdgeRenderPass::init(
    ID3D11Device* const device,
    ID3D11RenderTargetView* const render_target_view
) {
    if(!this->make_depth_stencil(device)) {
        return false;
    }

    if(!this->make_shader(device)) {
        return false;
    }

    if(!this->make_buffer(device)) {
        return false;
    }

    return true;
}

void EdgeRenderPass::update(ID3D11DeviceContext* const context) {
}

void EdgeRenderPass::render_set(
    ID3D11DeviceContext* const context,
    ID3D11RenderTargetView* const render_target_view
) const {
    context->OMSetRenderTargets(
        1,
        &render_target_view,
        this->resource->depth_stencil_view.at(Pattern::Model).Get()
    );
    context->OMSetDepthStencilState(
        this->resource->depth_stencil_state.at(Pattern::ModelEdge).Get(),
        0
    );

    context->IASetInputLayout(
        this->resource->input_layouts.at(Pattern::ModelEdge).Get()
    );

    // シェーダーのバインド
    context->VSSetShader(
        this->resource->vertex_shaders.at(Pattern::ModelEdge).Get(),
        nullptr,
        0
    );
    context->PSSetShader(
        this->resource->pixel_shaders.at(Pattern::ModelEdge).Get(),
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
            static_cast<uint32_t>(ConstantBufferName::ModelEdge)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::ModelEdge).GetAddressOf()
    );

    context->PSSetConstantBuffers(
        this->binding_slots->get(
            ShaderType::Pixel,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::ModelEdge)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::ModelEdge).GetAddressOf()
    );
}

void EdgeRenderPass::render(ID3D11DeviceContext* const context) const {
}

bool EdgeRenderPass::should_reset_state(void) const {
    return true;
}

bool EdgeRenderPass::is_render_model(void) const {
    return true;
}

bool EdgeRenderPass::is_post_render(void) const {
    return false;
}

void EdgeRenderPass::back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const {
}

RasterizerKind EdgeRenderPass::rasterizer_kind(void) const {
    return RasterizerKind::CullFront;
}

bool EdgeRenderPass::make_depth_stencil(ID3D11Device* const device) {
    {
        constexpr D3D11_DEPTH_STENCIL_DESC desc{
            .DepthEnable = TRUE,
            .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
            .DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
        };

        const HRESULT hr = device->CreateDepthStencilState(
            &desc,
            this->resource->depth_stencil_state[Pattern::ModelEdge].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"デプスステンシルステートの作成に失敗しました");
            return false;
        }
    }

    return true;
}

bool EdgeRenderPass::make_shader(ID3D11Device* const device) {
    Shader vertex_shader = Shader(std::make_unique<ModelEdgeVertexShader>());
    Shader pixel_shader = Shader(std::make_unique<ModelEdgePixelShader>());

    if(!pixel_shader.make_shader(
        device,
        this->resource->pixel_shaders[Pattern::ModelEdge].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_shader(
        device,
        this->resource->vertex_shaders[Pattern::ModelEdge].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_input_layout(
        device,
        this->resource->input_layouts[Pattern::ModelEdge].GetAddressOf()
    )) {
        return false;
    }

    // slot番号の取得
    this->binding_slots->merge(vertex_shader);
    this->binding_slots->merge(pixel_shader);

    return true;
}

bool EdgeRenderPass::make_buffer(ID3D11Device* const device) {
    // TODO: GUIから変更可能に
    constexpr ModelEdge edge{
        .edge_color = {0.0f, 0.0f, 0.0f},
        .edge_width = 0.0025f,
    };

    constexpr D3D11_BUFFER_DESC desc = {
        .ByteWidth = sizeof(decltype(edge)),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };

    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = &edge,
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(decltype(edge)),
    };

    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        this->resource->constant_buffers[ConstantBuffer::ModelEdge].GetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}