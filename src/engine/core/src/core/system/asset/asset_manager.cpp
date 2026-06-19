#include "asset_manager.h"

namespace enishi::core {
    void AssetManager::update(const types::DeltaTime& delta_time) {
    }

    std::vector<std::filesystem::path> core::AssetManager::find_assets(
        const std::filesystem::path& target_path,
        const std::unordered_set<std::filesystem::path>& target_extensions) {
        std::vector<std::filesystem::path> matched_files;
        std::vector<std::filesystem::path> directory_stack;
        std::error_code ec;

        // 初期ディレクトリの存在, 走査確認
        std::filesystem::path root_path(target_path);
        if (!std::filesystem::is_directory(root_path, ec) || ec) {
            return matched_files;
        }

        // 再帰的に探索
        directory_stack.push_back(root_path);
        while (!directory_stack.empty()) {
            const std::filesystem::path current_dir = directory_stack.back();
            directory_stack.pop_back();

            auto it = std::filesystem::directory_iterator(current_dir, ec);
            if (ec) {
                continue; // アクセス権限エラーなどはスキップ
            }

            for (const auto& entry : it) {
                const std::filesystem::file_status status = entry.status(ec);
                if (ec) {
                    continue;
                }

                if (std::filesystem::is_directory(status)) {
                    directory_stack.push_back(entry.path());
                } else if (std::filesystem::is_regular_file(status)) {
                    if (target_extensions.contains(entry.path().extension())) {
                        matched_files.push_back(entry.path().string());
                    }
                }
            }
        }

        return matched_files;
    }

    foundation::EngineResult<types::AssetHandle, assets_system::IOError>
    core::AssetManager::load_asset(const std::filesystem::path& path) noexcept {
        const auto result = assets_system::AssetLoader::load_model(path);

        if (result.is_err()) {
            return result;
        }

        return foundation::EngineResult<types::AssetHandle, IOError>();
    }

    void core::AssetManager::release_asset(const types::AssetHandle& handle) noexcept {
    }

    foundation::Option<const std::filesystem::path&> core::AssetManager::get_asset_file_name(
        const types::AssetHandle& handle) const noexcept {
        return foundation::Option<const std::filesystem::path&>();
    }
} // namespace enishi::core