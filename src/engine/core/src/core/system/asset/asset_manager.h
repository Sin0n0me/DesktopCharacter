#pragma once
#include "../interface_system.h"
#include <assets_system/interface_asset_system.h>
#include <ecs/registory.h>
#include <engine_types/assets/asset_handle.h>
#include <filesystem>
#include <foundation/str/str.h>
#include <unordered_map>
#include <unordered_set>

namespace enishi::core {
    class AssetManager : public ISystem, public assets_system::IAssetSystem {
      private:
        std::shared_ptr<ecs::Registory> registory;

      public:
        void update(const types::DeltaTime& delta_time) override;

      public:
        static std::vector<std::filesystem::path> find_assets(
            const std::filesystem::path& target_path,
            const std::unordered_set<std::filesystem::path>& target_extensions);

      public:
        foundation::EngineResult<types::AssetHandle, assets_system::IOError> load_asset(
            const std::filesystem::path& path) noexcept override;
        void release_asset(const types::AssetHandle& handle) noexcept override;
        foundation::Option<const std::filesystem::path&> get_asset_file_name(
            const types::AssetHandle& handle) const noexcept override;
    };
} // namespace enishi::core
