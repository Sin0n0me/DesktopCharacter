#include "render_pass_constructor.h"
#include <foundation/algorithm/resolve_dependencies.h>
#include <foundation/log/logger.h>

namespace enishi::render_pass {
    foundation::VoidResult<ConstructError> RenderPassConstructor::add_render_pass_constructor(
        std::shared_ptr<IRenderPassConstructor> render_pass_constructor) {
        if (!bool(render_pass_constructor)) {
            return foundation::Error(
                ConstructError::Construct, "レンダーパスコンストラクタがNULLです");
        }

        auto pass_name = render_pass_constructor->get_render_pass_name();
        if (this->name_to_index.contains(pass_name)) {
            foundation::Logger::info(
                std::format("レンダーパスが上書きされました. name: {}", pass_name));
        }

        this->asset_paths.append_range(
            render_pass_constructor->get_paths() | std::views::as_rvalue |
            std::views::transform(&std::filesystem::path::lexically_normal) |
            std::ranges::to<std::vector>());

        // hash_setで持って渡すより, ここで毎回重複を削除するほうが最終的にコストが軽い
        std::ranges::sort(this->asset_paths);
        auto ret = std::ranges::unique(this->asset_paths);
        this->asset_paths.erase(ret.begin(), ret.end());

        this->name_to_index[pass_name] = this->constructors.size();
        this->constructors.emplace_back(render_pass_constructor);
        return {};
    }

    foundation::Result<std::vector<std::shared_ptr<platform::IRenderPass>>, ConstructError>
    RenderPassConstructor::create_render_passes(
        std::vector<std::string>&& pass_names, platform::IRenderer* const renderer) {
        std::vector<foundation::DependencyDescription> dependencies;

        for (auto&& name : pass_names) {
            const auto iter = this->name_to_index.find(name);
            if (iter == this->name_to_index.end()) {
                foundation::Logger::warning(
                    std::format("存在しないレンダーパスが指定されました. name: {}", name));
                continue;
            }
            const auto& index = iter->second;
            const auto& constructor = this->constructors[index];

            dependencies.emplace_back(foundation::DependencyDescription{
                .node = constructor->get_node(),
                .bounds = constructor->get_dependencies(),
            });
        }

        auto&& result = foundation::resolve_dependencies(dependencies)
                            .add_message("レンダーパスの依存解決に失敗しました");
        if (result.is_err()) {
            return result.propagation(ConstructError::Construct);
        }

        // Topological order に従って RenderPass を生成
        auto& sorted_indices = result.unwrap();
        std::vector<std::shared_ptr<platform::IRenderPass>> render_passes;
        render_passes.reserve(sorted_indices.size());
        for (const auto& constructor_index : sorted_indices) {
            const auto& constructor = constructors[constructor_index];

            auto result =
                constructor->make(renderer).add_message("レンダーパスの構築に失敗しました");
            if (result.is_err()) {
                return result.propagation(ConstructError::Construct);
            }

            render_passes.emplace_back(std::move(result).unwrap());
        }

        return render_passes;
    }

    std::span<const std::filesystem::path> RenderPassConstructor::use_asset_paths(void) const {
        return this->asset_paths;
    }
} // namespace enishi::render_pass