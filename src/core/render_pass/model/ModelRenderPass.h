#pragma once
#include "../IRenderPass.h"
#include <wrl/client.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;

class ModelRenderPass : public IRenderPass {
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> model_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> model_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> model_input_layout;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadow_sampler;
	std::shared_ptr<CommonResource> common_resouce;

public:
	ModelRenderPass(const std::shared_ptr<CommonResource>& common_resouce);

public:
	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
	void render(ID3D11DeviceContext* const context) const override;

private:

	bool make_shaders(ID3D11Device* const device);
	bool make_depth_stencil(ID3D11Device* const device);
	bool make_shadow_sampler(ID3D11Device* const device);
};
