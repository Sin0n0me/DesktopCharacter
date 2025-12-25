#pragma once
#include "../IRenderPass.h"
#include <d3d11.h>
#include <wrl/client.h>

class ShadowRenderPass : public IRenderPass {
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadow_texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadow_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadow_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> shadow_input_layout;

	std::shared_ptr<CommonResource> common_resouce;
public:
	ShadowRenderPass(const std::shared_ptr<CommonResource>& common_resouce);

public:

	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
	void render(ID3D11DeviceContext* const context) const override;

private:

	bool make_shaders(ID3D11Device* const device);

	bool make_shadow_map(ID3D11Device* const device);
};
