#pragma once
#include <cstdint>

namespace enishi::ecs {
    using EntityID = std::uint32_t;
    constexpr EntityID NULL_ENTITY = UINT32_MAX;
} // namespace enishi::ecs
