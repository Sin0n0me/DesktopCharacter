#pragma once
#include <filesystem>
#include <foundation/result/result.h>
#include <foundation/str/str.h>
#include <memory>
#include <platform/asset/interface_asset_system.h>
#include <platform/renderer/interface_renderer.h>
#include <span>
#include <vector>

namespace enishi::model_controller {
    class ModelContoller {
      private:
        std::shared_ptr<platform::IAssetSystem> asset_system;
        std::shared_ptr<platform::IRenderer> renderer;
        std::unordered_map<foundation::UTF8, std::filesystem::path> model_list;
        std::vector<types::RenderHandle> model_shader_reflections;

      public:
        void find_model(void);

        foundation::Result<types::RenderHandle, int> change_model(const foundation::UTF8& name);

        [[nodiscard]] std::vector<foundation::UTF8> get_model_list(void);

        void action();

      private:
        void make_wall(void);
    };
} // namespace enishi::model_controller