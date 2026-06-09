#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace enishi::types {
    using BoneIndex = std::uint16_t;

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