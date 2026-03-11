#include "../../../../../Application.h"
#include "../../../../log/Logger.h"
#include "../../../CommonResource.h"
#include "../../../constant_buffer/ConstantBufferNames.h"
#include "../../../shader/model/PMDModelPixelShader.h"
#include "../../../shader/model/PMDModelVertexShader.h"
#include "../../../shader/Shader.h"
#include "ModelRenderPass.h"
#include <d3d11.h>

ModelRenderPass::ModelRenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept : RenderPass(common_resource) {}

bool ModelRenderPass::init(
    ID3D11Device* const device,
    ID3D11RenderTargetView* const render_target_view
) {
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
        constexpr DXGI_SAMPLE_DESC sample{
            .Count = 1
        };
        constexpr D3D11_TEXTURE2D_DESC desc{
            .Width = WIDTH,
            .Height = HEIGHT,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            .SampleDesc = sample,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        };

        const HRESULT hr = device->CreateTexture2D(
            &desc,
            nullptr,
            this->depth_texture.GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"デプスステンシル用のテクスチャ作成に失敗しました");
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
            Logger::error(u8"デプスステンシルビューの作成に失敗しました");
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
            this->resource->depth_stencil_state[Pattern::Model].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"デプスステンシルステートの作成に失敗しました");
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

RasterizerKind ModelRenderPass::rasterizer_kind(void) const {
    // MMDはポリゴンの両面を使用している
    // なので背面カリングすると表示がおかしくなる
    return RasterizerKind::CullNone;
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

bool ModelRenderPass::should_reset_state(void) const {
    return true;
}

bool ModelRenderPass::is_render_model(void) const {
    return true;
}

bool ModelRenderPass::is_post_render(void) const {
    return false;
}