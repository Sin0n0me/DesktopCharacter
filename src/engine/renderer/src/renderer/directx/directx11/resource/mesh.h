#pragma once
#include <engine_types/renderer/render_handle.h>
#include <vector>

namespace enishi::renderer::directx {
    struct Mesh {
        std::vector<types::RenderHandle> mesh_handles;
    };
} // namespace enishi::renderer::directx