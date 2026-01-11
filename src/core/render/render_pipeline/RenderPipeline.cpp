#include "../../../Application.h"
#include "../../D3D11.h"
#include "../model/Model.h"
#include "../render_pass/fxaa/FXAARenderPass.h"
#include "../render_pass/model/ModelRenderPass.h"
#include "../render_pass/shadow/ShadowRenderPass.h"
#include "../render_pass/wall/WallRenderPass.h"
#include "RenderPipeline.h"

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
	const int size = this->pipe_line.size();
	for(int i = 0; i < size; ++i) {
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
	D3D11_VIEWPORT view_port{};
	view_port.TopLeftX = 0.0f;
	view_port.TopLeftY = 0.0f;
	view_port.Width = static_cast<FLOAT>(WIDTH);
	view_port.Height = static_cast<FLOAT>(HEIGHT);
	view_port.MinDepth = 0.0f;
	view_port.MaxDepth = 1.0f;

	this->d3d11->context->ClearRenderTargetView(
		this->d3d11->render_target_view.Get(),
		CLEAR_COLOR
	);
	this->d3d11->context->ClearRenderTargetView(
		this->d3d11->render_target_view_back.Get(),
		CLEAR_COLOR
	);

	for(const auto& render_pass : this->pipe_line) {
		this->d3d11->context->RSSetViewports(1, &view_port);
		this->d3d11->context->RSSetState(this->d3d11->rasterizer_cull_back.Get());

		if(render_pass->is_post_render()) {
			render_pass->render_set(
				this->d3d11->context.Get(),
				this->d3d11->render_target_view_back.Get()
			);
			this->d3d11->context->PSSetShaderResources(
				0,
				1,
				this->d3d11->shader_resouce_view.GetAddressOf()
			);
		} else {
			render_pass->render_set(
				this->d3d11->context.Get(),
				this->d3d11->render_target_view.Get()
			);
		}

		render_pass->render(this->d3d11->context.Get());

		// モデルの描画が必要な場合
		if(render_pass->is_render_model() && bool(this->model)) {
			this->model->render(
				this->d3d11->context.Get(),
				render_pass->get_shader_binding_slots()
			);
		}

		ID3D11RenderTargetView* null_rtv[8] = {};
		this->d3d11->context->OMSetRenderTargets(8, null_rtv, nullptr);
		this->d3d11->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		this->d3d11->context->OMSetDepthStencilState(nullptr, 0);
		this->d3d11->context->IASetInputLayout(nullptr);
		this->d3d11->context->VSSetShader(nullptr, nullptr, 0);
		this->d3d11->context->PSSetShader(nullptr, nullptr, 0);
	}

	this->d3d11->dxgi_swap_chain->Present(1, 0);
}

void RenderPipeline::on_model_changed(const std::shared_ptr<Model>& model) {
	this->model = model;
}