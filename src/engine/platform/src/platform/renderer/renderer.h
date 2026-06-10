#pragma once
#include "../errors/renderer_errors.h"
#include <engine_types/assets/model/mesh_data.h>
#include <engine_types/assets/shader/shader_data.h>
#include <engine_types/assets/texture/texture_data.h>
#include <engine_types/renderer/render_graph.h>
#include <engine_types/renderer/render_handle.h>
#include <engine_types/window/window_handle.h>
#include <foundation/result/result.h>

namespace enishi::platform {
    template <typename T>
    using RenderResult = foundation::Result<T, foundation::Error<RenderError>>;

    class IRenderer {
      public:
        virtual ~IRenderer(void) noexcept = default;

        virtual RenderResult<void> init(const types::WindowHandle& window_handle) = 0;

        virtual RenderResult<types::RenderHandle> create_mesh(const types::MeshData& mesh) = 0;

        virtual RenderResult<types::RenderHandle> create_texture(
            const types::TextureData& texture) = 0;

        virtual RenderResult<types::RenderHandle> create_shader(
            const types::ShaderData& shader) = 0;

        virtual void submit_render_graph(const types::RenderGraph& graph) = 0;

        virtual void present(void) = 0;
    };
} // namespace enishi::platform