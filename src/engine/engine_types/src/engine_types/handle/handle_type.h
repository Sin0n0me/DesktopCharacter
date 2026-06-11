#pragma once
#include <cstdint>

namespace enishi::types {
    using HandleId = std::uint32_t; // バックエンド内の配列インデックス

    namespace details {
        constexpr std::uint32_t INITIAL_HANDLE_ID = 0;
        constexpr std::uint32_t INVALID_HANDLE_ID = UINT32_MAX;
    } // namespace details
} // namespace enishi::types