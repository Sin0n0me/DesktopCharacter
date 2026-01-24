#pragma once
#include "../shader/ShaderBindingSlots.h"
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct CommonResource;

class RenderPass {
protected:
    std::shared_ptr<CommonResource> resource;
    std::unique_ptr<ShaderBindingSlots> binding_slots;

public:
    explicit RenderPass(const std::shared_ptr<CommonResource>& common_resource);

    virtual ~RenderPass(void) = default;

    virtual bool init(
        ID3D11Device* const device,
        ID3D11RenderTargetView* const render_target_view
    ) = 0;

    virtual void update(ID3D11DeviceContext* const context) = 0;

    virtual void post_update(ID3D11DeviceContext* const context) {}

    virtual void render_set(
        ID3D11DeviceContext* const context,
        ID3D11RenderTargetView* const render_target_view
    ) const = 0;

    virtual void render(ID3D11DeviceContext* const context) const = 0;

    virtual bool should_reset_state(void) const = 0;

    virtual bool is_render_model(void) const = 0;

    virtual bool is_post_render(void) const = 0;

    virtual const ShaderBindingSlots* get_shader_binding_slots(void) const;

    virtual void back_buffer_resouce(
        ID3D11DeviceContext* const context,
        ID3D11ShaderResourceView* const shader_resouce_view
    ) const = 0;
};
