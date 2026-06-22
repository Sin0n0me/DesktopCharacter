#include "asset_manager.h"
#include <assets_system/model/model_loader/model_loader.h>

namespace enishi::core {
    foundation::Result<assets_system::AssetHandle, assets_system::AssetError>
    AssetManager::load_asset(const std::filesystem::path& path) noexcept {
        // すでにAssetを保持している場合はそのまま保管しているハンドルを返す
        const auto iter = this->path_to_handle.find(path);
        if (iter != this->path_to_handle.end()) {
            return iter->second;
        }

        if (path.has_extension()) {
            return foundation::Error(assets_system::AssetError::NotFound);
        }

        const auto extention = path.extension();
        const auto asset_iter = this->extension_to_asset_type.find(extention.string<char>());
        if (asset_iter == this->extension_to_asset_type.end()) {
            return foundation::Error(assets_system::AssetError::NotFound);
        }
        const assets_system::AssetType asset_type = asset_iter->second;

        types::HandleId id = types::INVALID_HANDLE_ID;
        switch (asset_type) {
            case assets_system::AssetType::Model: {
                const auto asset_id = this->load_model(path);
                if (asset_id.is_err()) {
                    return asset_id.propagation(assets_system::AssetError::IOError);
                }
                id = asset_id.value();
            } break;
            case assets_system::AssetType::Animation: {
                const auto asset_id = this->load_animation(path);
                if (asset_id.is_err()) {
                    return asset_id.propagation(assets_system::AssetError::IOError);
                }
                id = asset_id.value();
            } break;
            case assets_system::AssetType::Shader: {
            } break;
            case assets_system::AssetType::Texture: {
            } break;
            case assets_system::AssetType::Video: {
            } break;
            case assets_system::AssetType::Sound: {
            } break;
            case assets_system::AssetType::Script: {
            } break;
            default:
                break;
        }

        return assets_system::AssetHandle{
            .id = id,
            .type = asset_type,
        };
    }

    void AssetManager::release_asset(const assets_system::AssetHandle& handle) noexcept {
    }

    foundation::Option<const std::filesystem::path&> AssetManager::get_asset_file_name(
        const assets_system::AssetHandle& handle) const noexcept {
        return foundation::Option<const std::filesystem::path&>();
    }

    std::vector<std::filesystem::path> AssetManager::find_assets(
        const std::filesystem::path& target_path,
        const std::unordered_set<std::filesystem::path>& target_extensions) const noexcept {
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

    void core::AssetManager::update(const types::DeltaTime& delta_time) {
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_model(
        const std::filesystem::path& path) noexcept {
        auto model = assets_system::ModelLoader::load(path);
        if (model.is_err()) {
            return model.error();
        }

        const auto id = this->asset_registory.create();
        auto result = this->asset_registory.insert(id, model.value());
        if (result.is_err()) {
            // return model_.propagation();
        }

        return id;
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_animation(
        const std::filesystem::path& path) noexcept {
        return foundation::Result<types::HandleId, assets_system::IOError>();
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_shader(
        const std::filesystem::path& path) noexcept {
        return foundation::Result<types::HandleId, assets_system::IOError>();
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_texture(
        const std::filesystem::path& path) noexcept {
        return foundation::Result<types::HandleId, assets_system::IOError>();
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_video(
        const std::filesystem::path& path) noexcept {
        return foundation::Result<types::HandleId, assets_system::IOError>();
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_sound(
        const std::filesystem::path& path) noexcept {
        return foundation::Result<types::HandleId, assets_system::IOError>();
    }

    foundation::Result<types::HandleId, assets_system::IOError> AssetManager::load_script(
        const std::filesystem::path& path) noexcept {
        return foundation::Result<types::HandleId, assets_system::IOError>();
    }
} // namespace enishi::core