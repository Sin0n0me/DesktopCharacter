#pragma once
#include "../../../object/IObjectRenderer.h"
#include "../IRenderPass.h"
#include <wrl/client.h>

struct ID3D11Texture2D;

class FXAARenderPass : public IRenderPass {
private:
	std::shared_ptr<CommonResource> resource;
	std::unique_ptr<IObjectRenderer> quad_object;

	bool make_shader(ID3D11Device* const device);

public:
	FXAARenderPass(const std::shared_ptr<CommonResource>& common_resouce);

	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context) const override;
	void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
	bool is_render_model(void) const override;
	bool is_post_render(void) const override;
};
