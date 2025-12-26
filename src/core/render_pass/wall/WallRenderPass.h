#pragma once
#include "../IRenderPass.h"
#include <memory>
#include <wrl/client.h>
#include "../../object/IObjectRenderer.h"

struct ID3D11BlendState;

class WallRenderPass : public IRenderPass {
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
	std::shared_ptr<CommonResource> resource;
	std::unique_ptr<IObjectRenderer> wall_object; //

	WallRenderPass() = default;

public:
	WallRenderPass(const std::shared_ptr<CommonResource>& common_resource);

public:

	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context) const override;
	void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
	bool is_render_model(void) const override;

private:

	bool make_shaders(ID3D11Device* const device);
	bool make_blend_state(ID3D11Device* const device);
	bool make_depth_state(ID3D11Device* const device);
};
