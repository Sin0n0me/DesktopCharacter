#include "d3d11_renderer.h"

namespace enishi::renderer::directx {
    platform::RenderResult<void> D3D11Renderer::init(const types::WindowHandle& window_handle) {
        return {};
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_mesh(
        const types::MeshData& mesh) {
        return RenderResult<types::RenderHandle>();
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_texture(
        const types::TextureData& texture) {
        return RenderResult<types::RenderHandle>();
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_shader(
        const types::ShaderData& shader) {
        return RenderResult<types::RenderHandle>();
    }

    void D3D11Renderer::submit_render_graph(const types::RenderGraph& graph) {
        graph.camera;

        const auto context = this->d3d11->get_context();
        for (const auto& pass : graph.passes) {
            pass.pass_type;

            pass.render_target;

            // context->OMSetRenderTargets(8, null_rtv, nullptr);

            // context->ClearRenderTargetView(render_target_view.Get(), CLEAR_COLOR);

            // context->RSSetState();

            for (const auto& command : pass.commands) {
                command;
            }
        }
    }

    void D3D11Renderer::present(void) {
        this->d3d11->get_swap_chain()->Present(1, 0);
    }
} // namespace enishi::renderer::directx