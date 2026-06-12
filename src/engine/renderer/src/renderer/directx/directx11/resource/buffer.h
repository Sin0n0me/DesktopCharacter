#pragma once
#include "../shader/shader_type.h"
#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

namespace enishi::renderer::directx {
    enum class BufferType {
        Vertex,
        Index,
        Constant,
    };

    struct VertexParameter {
        std::uint32_t stride;
        std::uint32_t offset;
    };

    struct IndexParameter {
        DXGI_FORMAT format;
        std::uint32_t offset;
    };

    struct ConstantParameter {
        ShaderType target_shader;
        std::uint32_t target_slot;
    };

    // parameter
    union BufferParameter {
        VertexParameter vertex;
        IndexParameter index;
        ConstantParameter constant;
    };

    struct Buffer {
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
        BufferType buffer_type;
        BufferParameter parameter;
    };
} // namespace enishi::renderer::directx