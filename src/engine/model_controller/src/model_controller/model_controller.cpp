#pragma once
#include "model_controller.h"
#include <foundation/log/logger.h>
#include <foundation/path/path_utility.h>

namespace enishi::model_controller {
    void ModelContoller::find_model(void) {
        const auto pattern_model_extensions =
            this->asset_system->get_extensions_pattern(types::AssetKind::Model);
        const auto path = MODEL_PATH / "";
        const std::regex pattern(
            std::format("{}.*{}", foundation::path_to_regex_str(path), pattern_model_extensions));
        const auto model_paths =
            this->asset_system->find_assets(MODEL_PATH, types::AssetKind::Model);
        const auto asset_paths = model_paths.find(pattern);

        if (asset_paths.empty()) {
            return foundation::Error(ConstructError::Construct, "モデルデータが見つかりません");
        }

        for (const auto& asset_path : asset_paths) {
        }
    }

    void ModelContoller::change_model(const foundation::UTF8& name) {
        const auto iter = this->model_list.find(name);
        if (iter == this->model_list.end()) {
            return;
        }
        const auto& path = iter->second;

        auto&& result = this->asset_system->load_asset(path);
        if (result.is_err()) {
            continue;
        }
        const auto& handle = result.unwrap();

        const auto opt_model_data = this->asset_system->get_model_data(handle);
        if (opt_model_data.is_none()) {
            return;
        }
        const auto& model_data = opt_model_data.unwrap();

        // 先にテクスチャ読み込み
        for (const auto& material : model_data->materials) {
            for (const auto& material_texture : material.textures) {
                const auto asset_handle = this->asset_system->load_asset(material_texture.path);
                if (asset_handle.is_err()) {
                    foundation::Logger::warning(asset_handle.unwrap_err().get_message());
                }
            }
        }

        // メッシュ作成
        const auto mesh_handle = renderer->create_mesh(*model_data, this->model_shader_reflections);
        if (mesh_handle.is_err()) {
            return mesh_handle.unwrap_err();
        }
    }

    void ModelContoller::make_wall(void) {
        constexpr float WALL_SIZE = 30.0f;
        constexpr float HALF_WALL_SIZE = WALL_SIZE / 2.0f;
        constexpr float FRONT_DEPTH = 0.0f;
        constexpr float FLOOR_DEPTH = 1.5f; // 奥行(床面)
        constexpr float OFFSET_Y = 0.0f;

        auto model_data = std::make_shared<types::ModelData>();
        model_data->vertices = {
            types::VertexVariants{
                types::VertexPosition{
                    .position = {-HALF_WALL_SIZE, -HALF_WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH},
                },
            },
            types::VertexVariants{
                types::VertexPosition{
                    .position = {-HALF_WALL_SIZE, WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH},
                },
            },
            types::VertexVariants{
                types::VertexPosition{
                    .position = {HALF_WALL_SIZE, WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH},
                },
            },
            types::VertexVariants{
                types::VertexPosition{
                    .position = {HALF_WALL_SIZE, -HALF_WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH},
                },
            },
        };

        model_data->indices = {std::vector<std::uint16_t>{
            0,
            1,
            2,
            0,
            2,
            3,
        }};

        // メッシュ作成
        auto mesh_handle = renderer->create_mesh(*model_data, this->model_shader_reflections);
        if (mesh_handle.is_err()) {
            // return mesh_handle.propagation(core::SystemError::ConstructRenderPassError);
        }
    }
} // namespace enishi::model_controller