#pragma once
#include "errors/errors.h"
#include "model_render_data_builder.h"
#include <engine_types/handle/asset/asset_handle.h>
#include <engine_types/handle/renderer/render_handle.h>
#include <filesystem>
#include <foundation/option/option.h>
#include <foundation/result/result.h>
#include <foundation/str/str.h>
#include <memory>
#include <platform/asset/interface_asset_system.h>
#include <unordered_map>
#include <vector>

namespace enishi::model_controller {
    class ModelController {
      private:
        std::shared_ptr<platform::IAssetSystem> asset_system;
        std::shared_ptr<ModelRenderDataBuilder> builder;
        std::unordered_map<foundation::UTF8, std::filesystem::path> model_list;
        foundation::Option<foundation::UTF8> current_model_name;
        foundation::Option<types::RenderHandle> current_model_render_handle;

      public:
        ModelController(std::shared_ptr<platform::IAssetSystem> asset_system,
            std::shared_ptr<ModelRenderDataBuilder> builder) noexcept;

        // search_path以下からモデルアセットを探し、名前で選択できるよう一覧化する
        void find_model(const std::filesystem::path& search_path) noexcept;

        [[nodiscard]] std::vector<foundation::UTF8> get_model_list(void) const noexcept;

        [[nodiscard]] foundation::Option<foundation::UTF8> get_current_model_name(
            void) const noexcept;

        [[nodiscard]] foundation::Option<types::RenderHandle> get_current_model_render_handle(
            void) const noexcept;

        // 指定した名前のモデルへ切り替える
        // shader_reflectionsは描画データ作成に必要なため、呼び出し元(render_pass側)から受け取る
        [[nodiscard]] foundation::Result<types::RenderHandle, ControlError> change_model(
            const foundation::UTF8& name,
            const std::vector<types::RenderHandle>& shader_reflections) noexcept;

      private:
    };
} // namespace enishi::model_controller