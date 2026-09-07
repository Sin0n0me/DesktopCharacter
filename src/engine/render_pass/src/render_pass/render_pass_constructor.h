#pragma once
#include <filesystem>
#include <foundation/result/result.h>
#include <render_pass/constructor/interface_render_pass_construstor.h>
#include <render_pass/errors/errors.h>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace enishi::render_pass {
    class RenderPassConstructor {
      private:
        std::vector<std::filesystem::path> asset_paths;
        std::unordered_map<foundation::UTF8, std::size_t> name_to_index;
        std::vector<std::shared_ptr<IRenderPassConstructor>> constructors;

      public:
        [[nodiscard]] foundation::VoidResult<ConstructError> add_render_pass_constructor(
            std::shared_ptr<IRenderPassConstructor> render_pass_constructor);

        [[nodiscard]] foundation::Result<std::vector<std::shared_ptr<platform::IRenderPass>>,
            ConstructError>
        create_render_passes(
            std::vector<std::string>&& pass_names, platform::IRenderer* const renderer);

        // 事前読み込み用
        std::span<const std::filesystem::path> use_asset_paths(void) const;
    };
} // namespace enishi::render_pass