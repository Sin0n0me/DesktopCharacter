#include "../../../../Application.h"
#include "../../CommonResource.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../shader/model/PMDModelPixelShader.h"
#include "../../shader/model/PMDModelVertexShader.h"
#include "../../shader/Shader.h"
#include "ModelRenderPass.h"
#include <d3d11.h>

ModelRenderPass::ModelRenderPass(const std::shared_ptr<CommonResource>& common_resouce) noexcept : RenderPass(common_resouce) {}

bool ModelRenderPass::init(ID3D11Device* const device) {
    if(!this->make_depth_stencil(device)) {
        return false;
    }

    if(!this->make_shader(device)) {
        return false;
    }

    return true;
}

void ModelRenderPass::update(ID3D11DeviceContext* const context) {
}

void ModelRenderPass::render(ID3D11DeviceContext* const context) const {
}

// 深度ステンシルの作成
bool ModelRenderPass::make_depth_stencil(ID3D11Device* const device) {
    {
        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = WIDTH;
        texDesc.Height = HEIGHT;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        texDesc.SampleDesc.Count = 1;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        const HRESULT hr = device->CreateTexture2D(
            &texDesc,
            nullptr,
            this->depth_texture.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        const HRESULT hr = device->CreateDepthStencilView(
            this->depth_texture.Get(),
            nullptr,
            this->resource->depth_stencil_view[Pattern::Model].GetAddressOf()
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
            this->resource->depth_stencil_state[Pattern::Model].GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    return true;
}

bool ModelRenderPass::make_shader(ID3D11Device* const device) {
    Shader vertex_shader = Shader(std::make_unique<PMDModelVertexShader>());
    Shader pixel_shader = Shader(std::make_unique<PMDModelPixelShader>());

    if(!vertex_shader.make_shader(
        device,
        this->resource->vertex_shaders[Pattern::Model].GetAddressOf()
    )) {
        return false;
    }

    if(!vertex_shader.make_input_layout(
        device,
        this->resource->input_layouts[Pattern::Model].GetAddressOf()
    )) {
        return false;
    }

    if(!pixel_shader.make_shader(
        device,
        this->resource->pixel_shaders[Pattern::Model].GetAddressOf()
    )) {
        return false;
    }

    // slot番号の取得
    this->binding_slots->merge(vertex_shader);
    this->binding_slots->merge(pixel_shader);

    return true;
}

void ModelRenderPass::back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const {
}

void ModelRenderPass::render_set(
    ID3D11DeviceContext* const context,
    ID3D11RenderTargetView* const render_target_view
) const {
    context->ClearDepthStencilView(
        this->resource->depth_stencil_view.at(Pattern::Model).Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    context->OMSetRenderTargets(
        1,
        &render_target_view,
        this->resource->depth_stencil_view.at(Pattern::Model).Get()
    );

    context->OMSetDepthStencilState(
        this->resource->depth_stencil_state.at(Pattern::Model).Get(),
        0
    );

    // シェーダーのバインド
    context->IASetInputLayout(
        this->resource->input_layouts.at(Pattern::Model).Get()
    );
    context->VSSetShader(
        this->resource->vertex_shaders.at(Pattern::Model).Get(),
        nullptr,
        0
    );
    context->PSSetShader(
        this->resource->pixel_shaders.at(Pattern::Model).Get(),
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
}

bool ModelRenderPass::is_render_model(void) const {
    return true;
}

bool ModelRenderPass::is_post_render(void) const {
    return false;
}