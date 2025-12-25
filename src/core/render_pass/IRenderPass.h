#pragma once
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct CommonResource;

class IRenderPass {
protected:

public:
	IRenderPass(const std::shared_ptr<CommonResource> common_resource) {}

	virtual ~IRenderPass() = default;

	virtual bool init(ID3D11Device* const device) = 0;

	virtual void update(ID3D11DeviceContext* const context) = 0;

	virtual void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const = 0;

	virtual void render(ID3D11DeviceContext* const context) const = 0;
};
