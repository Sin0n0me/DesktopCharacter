#pragma once
#include "d3d11.h"
#include "resource/resource_manager.h"
#include "resource/shader.h"
#include <engine_types/handle/handle_allocator.h>
#include <engine_types/renderer/render_handle.h>
#include <memory>
#include <platform/renderer/renderer.h>
#include <unordered_map>

namespace enishi::renderer::directx {
    class D3D11Renderer : public platform::IRenderer {
      private:
        std::unique_ptr<D3D11> d3d11;
        ResourceManager resource_manager;

      private:
        void bind(ID3D11DeviceContext* const context, const types::DrawCommand& command) const;
        void bind_buffer(ID3D11DeviceContext* const context, const Buffer& buffer) const;
        void bind_shader(ID3D11DeviceContext* const context, const types::HandleId id) const;

      public:
        explicit D3D11Renderer(std::unique_ptr<D3D11> d3d11);

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
