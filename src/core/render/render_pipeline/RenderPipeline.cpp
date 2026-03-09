#include "../../../Application.h"
#include "../../D3D11.h"
#include "../../log/Logger.h"
#include "../model/Model.h"
#include "../render_pass/alpha_mask/AlphaMaskRenderPass.h"
#include "../render_pass/edge/EdgeRenderPass.h"
#include "../render_pass/fxaa/FXAARenderPass.h"
#include "../render_pass/model/ModelRenderPass.h"
#include "../render_pass/shadow/ShadowRenderPass.h"
#include "../render_pass/wall/WallRenderPass.h"
#include "RenderPipeline.h"

constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
//constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 1.0f}; //アルファマスク使用時はこっち

RenderPipeline::RenderPipeline(
    const std::shared_ptr<D3D11>& d3d11,
    const std::shared_ptr<CommonResource>& resouce
) noexcept : d3d11(d3d11) {
    // レンダーパスの追加(ここではあくまで追加だけ後続のinit内で任意の順序にする)
    this->render_pass_map.emplace(
        RenderPassName::ShadowMap,
        std::make_unique<ShadowRenderPass>(resouce)
    );
    this->render_pass_map.emplace(
        RenderPassName::Wall,
        std::make_unique<WallRenderPass>(resouce)
    );
    this->render_pass_map.emplace(
        RenderPassName::Model,
        std::make_unique<ModelRenderPass>(resouce)
    );
    this->render_pass_map.emplace(
        RenderPassName::FXAA,
        std::make_unique<FXAARenderPass>(resouce)
    );
    this->render_pass_map.emplace(
        RenderPassName::AlphaMask,
        std::make_unique<AlphaMaskRenderPass>(resouce)
    );
    this->render_pass_map.emplace(
        RenderPassName::Edge,
        std::make_unique<EdgeRenderPass>(resouce)
    );
}

bool RenderPipeline::contains(const RenderPassName& name) {
    const auto& find_pass = this->render_pass_map.at(name);
    for(const auto& render_pass : this->pipe_line) {
        if(render_pass == find_pass) {
            return true;
        }
    }

    return false;
}

void RenderPipeline::push_back(const RenderPassName& name) {
    if(this->contains(name)) {
        return;
    }
    this->pipe_line.push_back(this->render_pass_map.at(name));
}

void RenderPipeline::push_front(const RenderPassName& name) {
    if(this->contains(name)) {
        return;
    }
    this->pipe_line.push_front(this->render_pass_map.at(name));
}

void RenderPipeline::insert(const RenderPassName& name, const uint32_t& index) {
    if(this->contains(name)) {
        return;
    }
    this->pipe_line.insert(this->pipe_line.begin() + index, this->render_pass_map.at(name));
}

void RenderPipeline::remove(const RenderPassName& name) {
    const size_t size = this->pipe_line.size();
    for(size_t i = 0; i < size; ++i) {
        const auto& pass = this->pipe_line.front();
        this->pipe_line.pop_front();

        if(pass == this->render_pass_map.at(name)) {
            this->pipe_line.push_back(pass);
        }
    }
}

void RenderPipeline::set(const std::vector<RenderPassName>& names) {
    for(const auto& name : names) {
        this->push_back(name);
    }
}

// ラスタライザの作成
bool RenderPipeline::make_rasterizer(void) {
    constexpr INT DEPTH_BIAS = 100;
    constexpr FLOAT SLOPE_SCALED_DEPTH_BIAS = 0.5f;
    constexpr BOOL DEPTH_CLIP_EBABLE = TRUE;

    {
        constexpr D3D11_RASTERIZER_DESC desc{
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_NONE,
            .FrontCounterClockwise = FALSE,
            .DepthBias = DEPTH_BIAS,
            .SlopeScaledDepthBias = SLOPE_SCALED_DEPTH_BIAS,
            .DepthClipEnable = DEPTH_CLIP_EBABLE,
        };

        const HRESULT hr = this->d3d11->device->CreateRasterizerState(
            &desc,
            this->rasterizer[RasterizerKind::CullNone].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"ラスタライザの作成に失敗しました");
            return false;
        }
    }

    {
        constexpr D3D11_RASTERIZER_DESC desc{
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_BACK,
            .FrontCounterClockwise = FALSE,
            .DepthBias = DEPTH_BIAS,
            .SlopeScaledDepthBias = SLOPE_SCALED_DEPTH_BIAS,
            .DepthClipEnable = DEPTH_CLIP_EBABLE,
        };

        const HRESULT hr = this->d3d11->device->CreateRasterizerState(
            &desc,
            this->rasterizer[RasterizerKind::CullBack].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"ラスタライザの作成に失敗しました");
            return false;
        }
    }

    {
        constexpr D3D11_RASTERIZER_DESC desc{
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_FRONT,
            .FrontCounterClockwise = FALSE,
            .DepthBias = DEPTH_BIAS,
            .SlopeScaledDepthBias = SLOPE_SCALED_DEPTH_BIAS,
            .DepthClipEnable = DEPTH_CLIP_EBABLE,
        };

        const HRESULT hr = this->d3d11->device->CreateRasterizerState(
            &desc,
            this->rasterizer[RasterizerKind::CullFront].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"ラスタライザの作成に失敗しました");
            return false;
        }
    }

    return true;
}

// RenderTargetView作成
bool RenderPipeline::make_render_target_views(void) {
    if(!this->make_offscreen_render_target_view()) {
        return false;
    }

    if(!this->make_back_buffer_render_target_view()) {
        return false;
    }

    return true;
}

bool RenderPipeline::make_offscreen_render_target_view(void) {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    {
        constexpr DXGI_SAMPLE_DESC sample{
            .Count = 1
        };
        constexpr D3D11_TEXTURE2D_DESC desc{
            .Width = WIDTH,
            .Height = HEIGHT,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = sample,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
        };

        const HRESULT hr = this->d3d11->device->CreateTexture2D(
            &desc,
            nullptr,
            texture.GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"レンダーターゲット用のテクスチャ作成に失敗しました");
            return false;
        }
    }

    {
        const HRESULT hr = this->d3d11->device->CreateRenderTargetView(
            texture.Get(),
            nullptr,
            this->render_target_view[RenderTargetView::Offscreen].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"レンダーターゲット(オフスクリーン)の作成に失敗しました");
            return false;
        }
    }

    {
        const HRESULT hr = this->d3d11->device->CreateShaderResourceView(
            texture.Get(),
            nullptr,
            this->shader_resouce_view.GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"シェーダーリソースビューの作成に失敗しました");
            return false;
        }
    }

    return true;
}

bool RenderPipeline::make_back_buffer_render_target_view(void) {
    // バックバッファ取得
    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
    {
        const HRESULT hr = this->d3d11->dxgi_swap_chain->GetBuffer(
            0,
            IID_PPV_ARGS(back_buffer.GetAddressOf())
        );
        if(FAILED(hr)) {
            Logger::error(u8"バックバッファの取得に失敗しました");
            return false;
        }
    }

    {
        const HRESULT hr = this->d3d11->device->CreateRenderTargetView(
            back_buffer.Get(),
            nullptr,
            this->render_target_view[RenderTargetView::BackBuffer].GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"レンダーターゲット(バックバッファ)の作成に失敗しました");
            return false;
        }
    }

    return true;
}

void RenderPipeline::reset_state(void) const {
    static ID3D11RenderTargetView* const null_rtv[8] = {};
    static ID3D11ShaderResourceView* const null_srv[8] = {};
    static ID3D11SamplerState* const null_sampler[8] = {};
    constexpr D3D11_VIEWPORT view_port{
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = static_cast<FLOAT>(WIDTH),
        .Height = static_cast<FLOAT>(HEIGHT),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    const auto& context = this->d3d11->context;

    context->OMSetRenderTargets(8, null_rtv, nullptr);
    context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(nullptr, 0);
    context->IASetInputLayout(nullptr);
    context->VSSetShader(nullptr, nullptr, 0);
    context->PSSetShader(nullptr, nullptr, 0);
    context->CSSetShader(nullptr, nullptr, 0);
    context->PSSetShaderResources(0, 8, null_srv);
    context->VSSetShaderResources(0, 8, null_srv);
    context->CSSetShaderResources(0, 8, null_srv);
    context->VSSetSamplers(0, 8, null_sampler);
    context->PSSetSamplers(0, 8, null_sampler);
    context->CSSetSamplers(0, 8, null_sampler);
    context->RSSetViewports(1, &view_port);
}

bool RenderPipeline::init(void) {
    if(!this->make_render_target_views()) {
        return false;
    }

    if(!this->make_rasterizer()) {
        return false;
    }

    // レンダーパスの初期化
    for(auto& [index, render_pass] : this->render_pass_map) {
        const auto render_target = [&]() {
            if(render_pass->is_post_render()) {
                return this->render_target_view.at(RenderTargetView::BackBuffer);
            }

            return this->render_target_view.at(RenderTargetView::Offscreen);
            }();

        if(!render_pass->init(this->d3d11->device.Get(), render_target.Get())) {
            Logger::error(u8"レンダーパスの初期化に失敗しました");
            return false;
        }
    }

    // 描画順序
    this->set({
        RenderPassName::ShadowMap,
        RenderPassName::Wall,
        RenderPassName::Model,
        RenderPassName::Edge,
        RenderPassName::FXAA,
        });

    return true;
}

void RenderPipeline::render_update(void) {
    for(auto& render_pass : this->pipe_line) {
        render_pass->update(this->d3d11->context.Get());
    }
}

void RenderPipeline::render(void) const {
    const auto& context = this->d3d11->context;

    for(const auto& [_, render_target_view] : this->render_target_view) {
        context->ClearRenderTargetView(
            render_target_view.Get(),
            CLEAR_COLOR
        );
    }

    for(const auto& render_pass : this->pipe_line) {
        if(render_pass->should_reset_state()) {
            this->reset_state();
        }

        context->RSSetState(
            this->rasterizer.at(render_pass->rasterizer_kind()).Get()
        );

        if(render_pass->is_post_render()) {
            render_pass->render_set(
                context.Get(),
                this->render_target_view.at(RenderTargetView::BackBuffer).Get()
            );
            render_pass->back_buffer_resouce(
                context.Get(),
                this->shader_resouce_view.Get()
            );
        } else {
            render_pass->render_set(
                context.Get(),
                this->render_target_view.at(RenderTargetView::Offscreen).Get()
            );
        }

        render_pass->render(context.Get());

        // モデルの描画が必要な場合
        if(render_pass->is_render_model() && bool(this->model)) {
            this->model->render(
                this->d3d11->context.Get(),
                render_pass->get_shader_binding_slots()
            );
        }
    }

    this->d3d11->dxgi_swap_chain->Present(1, 0);
}

void RenderPipeline::on_model_changed(const std::shared_ptr<Model>& model) {
    this->model = model;
}