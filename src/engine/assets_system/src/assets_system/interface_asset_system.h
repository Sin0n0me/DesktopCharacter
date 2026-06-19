#pragma once
#include "asset_handle.h"
#include "errors/errors.h"
#include <filesystem>
#include <foundation/option/option.h>
#include <foundation/result/result.h>

namespace enishi::assets_system {
    class IAssetSystem {
      public:
        virtual ~IAssetSystem(void) noexcept = default;

        virtual foundation::EngineResult<AssetHandle, IOError> load_asset(
            const std::filesystem::path& path) noexcept = 0;

        virtual void release_asset(const AssetHandle& handle) noexcept = 0;

        virtual foundation::Option<const std::filesystem::path&> get_asset_file_name(
            const AssetHandle& handle) const noexcept = 0;

        virtual foundation::Option<const int&> a();
    };
} // namespace enishi::assets_system
