#pragma once
#include "../RenderPass.h"
#include <wrl/client.h>

struct ID3D11Texture2D;

class ShadowRenderPass : public RenderPass {
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> shadow_texture;

public:
    explicit ShadowRenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept;

public:

    bool init(ID3D11Device* const device, ID3D11RenderTargetView* const render_target_view) override;
    void update(ID3D11DeviceContext* const context) override;
    void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
    void render(ID3D11DeviceContext* const context) const override;
    bool should_reset_state(void) const override;
    bool is_render_model(void) const override;
    bool is_post_render(void) const override;
    void back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const override;
    RasterizerKind rasterizer_kind(void) const override;

private:

    bool make_shaders(ID3D11Device* const device);

    bool make_shadow_map(ID3D11Device* const device);
};
