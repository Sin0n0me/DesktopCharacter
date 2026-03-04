#pragma once
#include "../../utility/Hash.h"

constexpr char ALPHA_MASK_UAV_NAME[] = "alpha_mask";

enum class UnorderedAccessViewName : uint32_t {
    AlphaMask = hash_u32(ALPHA_MASK_UAV_NAME),
};
