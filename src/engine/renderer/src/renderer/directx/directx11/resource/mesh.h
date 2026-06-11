#pragma once
#include <engine_types/renderer/render_handle.h>

namespace enishi::renderer::directx {
    struct Mesh {
        types::RenderHandle vertex_handle;
        types::RenderHandle index_handle;
    };
} // namespace enishi::renderer::directx