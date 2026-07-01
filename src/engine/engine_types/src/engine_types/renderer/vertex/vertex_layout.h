#pragma once
#include <cstdint>
#include <string_view>
#include <vector>

namespace enishi::types {
    // 1要素のフォーマット
    enum class VertexFormat : std::uint8_t {
        Float1, // float
        Float2, // glm::vec2
        Float3, // glm::vec3
        Float4, // glm::vec4
        UInt4,  // glm::uvec4(ボーンインデックス用)
        UNorm4, // 8bit×4 正規化(色など)
    };

    // セマンティクス
    // DirectX の POSITION, NORMAL に相当
    enum class VertexSemantic : std::uint8_t {
        Position,
        Normal,
        Tangent,
        TexCoord0,
        TexCoord1,
        Color,
        BoneWeights,
        BoneIndices,
    };

    // 1頂点属性の定義
    // DirectX の D3D11_INPUT_ELEMENT_DESC 1要素に相当
    struct VertexAttribute {
        VertexSemantic semantic;
        VertexFormat format;
        std::uint32_t offset;  // 頂点構造体先頭からのバイトオフセット
        std::uint32_t binding; // バインディング番号（複数VBOの場合）
    };

    // 頂点バッファ1本分のストライド定義
    struct VertexBinding {
        std::uint32_t binding; // バインディング番号
        std::uint32_t stride;  // 1頂点のバイト数
    };

    // 頂点レイアウト全体
    // DirectX の InputLayout 全体に相当
    struct VertexLayout {
        std::vector<VertexBinding> bindings;
        std::vector<VertexAttribute> attributes;

        // フォーマットのバイトサイズを返すヘルパー
        [[nodiscard]]
        static constexpr std::uint32_t format_size(const VertexFormat fmt) noexcept {
            switch (fmt) {
                case VertexFormat::Float1:
                    return 4;
                case VertexFormat::Float2:
                    return 8;
                case VertexFormat::Float3:
                    return 12;
                case VertexFormat::Float4:
                    return 16;
                case VertexFormat::UInt4:
                    return 16;
                case VertexFormat::UNorm4:
                    return 4;
                default:
                    return 0;
            }
        }
    };
} // namespace enishi::types