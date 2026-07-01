#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace enishi::types {
    enum class ShaderFileType {
        HLSL,
        GLSL,
        MSL,
    };

    enum class ShaderBinaryType {
        SPIR_V,
        DXIL,
        DXBC,

        SourceCode,
    };

    struct ShaderData {
        ShaderBinaryType binary_type;
        std::vector<std::uint8_t> code;
    };
} // namespace enishi::types