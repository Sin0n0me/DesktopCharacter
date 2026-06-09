#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace enishi::types {
    enum class VertexType : std::uint8_t {
        Uint,
        Int,
        Float,
    };

    enum class VertexStride : std::uint8_t {
        Stride8bit,
        Stride16bit,
        Stride32bit,
        Stride64bit,
    };

    enum class IndexStride {
        Stride8bit,
        Stride16bit,
        Stride32bit,
        Stride64bit,
    };

    // 余分なデータも含むのでRender内で最適な構造体に変換
    struct Vertex {
        glm::vec4 position;
        glm::vec4 color;
        glm::vec2 uv;
        glm::vec4 normal;
    };

    struct VertexFormat {
        VertexType type;
        VertexStride stride;
    };

    struct MeshData {
        std::vector<VertexFormat> vertex_format;
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
        IndexStride indices_stride;
    };
} // namespace enishi::types
