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
    }

    void D3D11Renderer::present(void) {
    }
} // namespace enishi::renderer::directx