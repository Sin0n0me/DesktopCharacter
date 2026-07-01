#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace enishi::types {
    using MorphIndex = std::uint16_t;

    constexpr MorphIndex INVALID_MORPH_INDEX = UINT16_MAX;
} // namespace enishi::types