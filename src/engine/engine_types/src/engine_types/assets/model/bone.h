#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace enishi::types {
    // 複数のモデルを扱う場合 std::uint16_t だと不足する可能性が高いため
    // std::uint32_t になっている
    using BoneIndex = std::uint32_t;

    struct BineBone {
        glm::mat4 local;          // ローカル
        glm::mat4 global;         //
        glm::mat4 global_inverse; // スキニング計算に使用
    };

    struct BoneNode {
        std::vector<BoneIndex> children;
        glm::mat4 local;
        glm::mat4 global;
    };
} // namespace enishi::types