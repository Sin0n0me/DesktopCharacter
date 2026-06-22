#pragma once
#include <cstdint>
#include <engine_types/handle/handle_type.h>

namespace enishi::ecs {
    using EntityID = types::HandleId;
    constexpr EntityID NULL_ENTITY = UINT32_MAX;
} // namespace enishi::ecs
