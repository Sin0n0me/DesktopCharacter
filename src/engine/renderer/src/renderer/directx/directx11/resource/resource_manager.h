#pragma once
#include "../../../errors/errors.h"
#include "gpu_resource.h"
#include "mesh.h"
#include <engine_types/assets/model/mesh_data.h>
#include <engine_types/handle/handle_allocator.h>
#include <engine_types/renderer/render_data.h>
#include <engine_types/renderer/render_handle.h>
#include <foundation/result/result.h>
#include <optional>
#include <unordered_map>

namespace enishi::renderer::directx {
    class ResourceManager {
      private:
        types::HandleAllocator handle_allocator;
        std::unordered_map<types::HandleId, types::RenderHandleType> handles;
        std::unordered_map<types::HandleId, Mesh> meshes;
        GPUResource resource;

      public:
        using Result = foundation::EngineResult<types::RenderHandle, DirectXError>;

        Result make_mesh(ID3D11Device* const device, const types::MeshData& mesh);
        Result make_vertex_buffer(ID3D11Device* const device, const types::RenderData& data);
        Result make_index_buffer(ID3D11Device* const device, const types::RenderData& data);
        Result make_constant_buffer(ID3D11Device* const device, const types::RenderData& data);
        Result make_texture(ID3D11Device* const device,
            const types::RenderData& data,
            const std::uint32_t width,
            const std::uint32_t height);
        Result make_blend_state();
        Result make_sampler();
        Result make_rasterizer();
        Result make_render_target_view();

        std::optional<const Buffer&> get_buffer(const types::HandleId handle) const;
        std::optional<const Texture&> get_texture(const types::HandleId handle) const;
        const ShaderPool& get_shader_pool(void) const;
    };
} // namespace enishi::renderer::directx