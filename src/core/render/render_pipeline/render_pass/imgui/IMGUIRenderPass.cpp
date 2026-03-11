#include "../../../../Core.h"
#include "../../../../gui/IMGUIManager.h"
#include "../../../../log/Logger.h"
#include "IMGUIRenderPass.h"

IMGUIRenderPass::IMGUIRenderPass(
    ID3D11Device* const device,
    ID3D11DeviceContext* const context,
    const std::shared_ptr<CommonResource>& common_resource
) noexcept :
    RenderPass(common_resource) {
    this->gui_manager = std::make_unique<IMGUIManager>();
}

bool IMGUIRenderPass::init(ID3D11Device* const device, ID3D11RenderTargetView* const render_target_view) {
    /*
    if(!this->gui_manager->init(
        root.hwnd,
        this->d3d11->device.Get(),
        this->d3d11->context.Get()
    )) {
        Logger::error(u8"GUIの初期化に失敗しました");
        return false;
    }
    */

    return true;
}

void IMGUIRenderPass::update(ID3D11DeviceContext* const context) {
}

void IMGUIRenderPass::render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const {
}

void IMGUIRenderPass::render(ID3D11DeviceContext* const context) const {
}

bool IMGUIRenderPass::should_reset_state(void) const {
    return false;
}

bool IMGUIRenderPass::is_render_model(void) const {
    return false;
}

bool IMGUIRenderPass::is_post_render(void) const {
    return false;
}

void IMGUIRenderPass::back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const {
}

RasterizerKind IMGUIRenderPass::rasterizer_kind(void) const {
    return RasterizerKind::CullBack;
}