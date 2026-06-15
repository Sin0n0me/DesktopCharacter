#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace enishi::types {
    using BoneIndex = std::uint16_t;

    constexpr BoneIndex INVALID_BONE_INDEX = UINT16_MAX;

    struct BindBone {
        glm::mat4 local;          // ローカル
        glm::mat4 global;         //
        glm::mat4 global_inverse; // スキニング計算に使用
    };

    struct BoneNode {
        BoneIndex parent;
        std::vector<BoneIndex> children;

        bool has_parent(void) const;
    };

    struct B {
        glm::mat4 local;
        glm::mat4 global;
    };
} // namespace enishi::types