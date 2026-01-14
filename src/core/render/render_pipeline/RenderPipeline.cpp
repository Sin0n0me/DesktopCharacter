#include "../../../Application.h"
#include "../../D3D11.h"
#include "../model/Model.h"
#include "../render_pass/fxaa/FXAARenderPass.h"
#include "../render_pass/model/ModelRenderPass.h"
#include "../render_pass/shadow/ShadowRenderPass.h"
#include "../render_pass/wall/WallRenderPass.h"
#include "RenderPipeline.h"
#include <iostream>

RenderPipeline::RenderPipeline(
    const std::shared_ptr<D3D11>& d3d11,
    const std::shared_ptr<CommonResource>& resouce
) noexcept : d3d11(d3d11) {
    // レンダーパスの追加
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

bool RenderPipeline::init(void) {
    // レンダーパスの初期化
    for(auto& [index, render_pass] : this->render_pass_map) {
        if(!render_pass->init(this->d3d11->device.Get())) {
            return false;
        }
    }

    this->set({
        RenderPassName::ShadowMap,
        RenderPassName::Wall,
        RenderPassName::Model,
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
    constexpr D3D11_VIEWPORT view_port{
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = static_cast<FLOAT>(WIDTH),
        .Height = static_cast<FLOAT>(HEIGHT),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    ID3D11RenderTargetView* const null_rtv[8] = {};
    ID3D11ShaderResourceView* const null_srv[8] = {};
    ID3D11SamplerState* const null_sampler[8] = {};

    const auto& context = this->d3d11->context;

    context->ClearRenderTargetView(
        this->d3d11->render_target_view.Get(),
        CLEAR_COLOR
    );
    context->ClearRenderTargetView(
        this->d3d11->render_target_view_back.Get(),
        CLEAR_COLOR
    );

    for(const auto& render_pass : this->pipe_line) {
        context->OMSetRenderTargets(8, null_rtv, nullptr);
        context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        context->OMSetDepthStencilState(nullptr, 0);
        context->IASetInputLayout(nullptr);
        context->VSSetShader(nullptr, nullptr, 0);
        context->PSSetShader(nullptr, nullptr, 0);
        context->PSSetShaderResources(0, 8, null_srv);
        context->VSSetShaderResources(0, 8, null_srv);
        context->PSSetSamplers(0, 8, null_sampler);

        context->RSSetViewports(1, &view_port);
        context->RSSetState(this->d3d11->rasterizer_cull_back.Get());

        if(render_pass->is_post_render()) {
            render_pass->render_set(
                context.Get(),
                this->d3d11->render_target_view_back.Get()
            );
            render_pass->back_buffer_resouce(
                context.Get(),
                this->d3d11->shader_resouce_view.Get()
            );
        } else {
            render_pass->render_set(
                context.Get(),
                this->d3d11->render_target_view.Get()
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