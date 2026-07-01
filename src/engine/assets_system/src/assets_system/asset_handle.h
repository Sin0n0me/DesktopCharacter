#pragma once
#include "asset_type.h"
#include <cstdint>
#include <engine_types/handle/handle_type.h>
#include <functional>

namespace enishi::assets_system {
    struct AssetHandle {
        types::HandleId id;
        AssetType type;

        bool is_valid(void) const noexcept;

        // キーとして扱えるようにハッシュ化
        [[nodiscard]]
        size_t operator()(const AssetHandle& h) const noexcept {
            return std::hash<std::uint64_t>{}(
                (static_cast<std::uint64_t>(h.type) << (sizeof(decltype(h.id)) * 8)) |
                static_cast<std::uint64_t>(h.id));
        }
    };
} // namespace enishi::assets_system
