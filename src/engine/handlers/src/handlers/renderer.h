#pragma once

#include "../window_handle.h"
#include <types>

namespace enishi::handler {
    class IRenderer {
      public:
        virtual ~IRenderer(void) noexcept = default;

        virtual void init(const window::WindowHandle& window_handle) = 0;

        virtual RenderHandle create_mesh(const MeshData&) = 0;

        virtual RenderHandle create_texture(const TextureData&) = 0;

        virtual RenderHandle create_shader(const TextureData&) = 0;

        virtual RenderHandle add_shader(const TextureData&) = 0;

        virtual void submit_render_graph(const RenderGraph&) = 0;

        virtual void present() = 0;
    };
} // namespace enishi::handler