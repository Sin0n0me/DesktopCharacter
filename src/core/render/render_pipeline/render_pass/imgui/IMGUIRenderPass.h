#pragma once
#include "../RenderPass.h"

class IMGUIManager;

class IMGUIRenderPass : public RenderPass {
private:
    std::unique_ptr<IMGUIManager> gui_manager;

public:
    explicit IMGUIRenderPass(
        ID3D11Device* const device,
        ID3D11DeviceContext* const context,
        const std::shared_ptr<CommonResource>& common_resource
    ) noexcept;

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
};
