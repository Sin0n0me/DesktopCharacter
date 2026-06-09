#pragma once
#include <engine_types/renderer/render_handle.h>
#include <platform/handler/renderer/renderer.h>

namespace enishi::renderer::directx {
    class D3D11Renderer : public platform::IRenderer {
      private:
      public:
        platform::RenderResult<void> init(const types::WindowHandle& window_handle) override;
        platform::RenderResult<types::RenderHandle> create_mesh(
            const types::MeshData& mesh) override;
        platform::RenderResult<types::RenderHandle> create_texture(
            const types::TextureData& texture) override;
        platform::RenderResult<types::RenderHandle> create_shader(
            const types::ShaderData& shader) override;
        void submit_render_graph(const types::RenderGraph& graph) override;
        void present(void) override;
    };
} // namespace enishi::renderer::directx
