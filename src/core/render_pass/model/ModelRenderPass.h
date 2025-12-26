#pragma once
#include "../IRenderPass.h"
#include <wrl/client.h>

struct ID3D11Texture2D;

class ModelRenderPass : public IRenderPass {
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_texture;
	std::shared_ptr<CommonResource> resource;

public:
	ModelRenderPass(const std::shared_ptr<CommonResource>& common_resouce);

public:
	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
	void render(ID3D11DeviceContext* const context) const override;
	bool is_render_model(void) const override;

private:

	bool make_shaders(ID3D11Device* const device);
	bool make_depth_stencil(ID3D11Device* const device);
};
