#pragma once
#include "buffer.h"
#include "shader.h"
#include "texture.h"
#include <d3d11.h>
#include <engine_types/handle/handle_type.h>
#include <unordered_map>
#include <wrl/client.h>

namespace enishi::renderer::directx {
    struct GPUResource {
        std::unordered_map<types::HandleId, Buffer> buffers;
        std::unordered_map<types::HandleId, Texture> textures;
        ShaderPool shaders;
    };
} // namespace enishi::renderer::directx