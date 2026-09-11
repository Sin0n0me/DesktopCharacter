#pragma once
#include <engine_types/assets/asset_data.h>
#include <engine_types/assets/shader/shader_data.h>
#include <engine_types/assets/shader/shader_kind.h>
#include <filesystem>
#include <foundation/result/result.h>
#include <foundation/str/str.h>
#include <platform/errors/renderer_errors.h>
#include <span>
#include <unordered_map>
#include <vector>

namespace enishi::platform {
    struct ShaderDataEntry {
        types::ShaderKind kind;
        std::filesystem::path path;
        types::AssetShaderData data;
    };

    class IShaderDataProvider {
      public:
        virtual ~IShaderDataProvider(void) noexcept = default;

        [[nodiscard]]
        virtual foundation::Result<std::vector<ShaderDataEntry>, RenderError> get(
            std::span<const std::tuple<types::ShaderKind, std::filesystem::path>> paths) const = 0;
    };

    using ShaderKindToData =
        std::unordered_map<types::ShaderKind, std::vector<types::AssetShaderData>>;

    ShaderKindToData make_shader_map_presorted(const std::vector<ShaderDataEntry>& entries) {
        ShaderKindToData map;

        auto projection = entries | std::views::transform([](const auto& entry) {
            return std::pair{entry.kind, entry.data};
        });

        for (auto group : projection | std::views::chunk_by([](const auto& a, const auto& b) {
                 return a.first == b.first;
             })) {
            types::ShaderKind kind = group.front().first;
            auto data_list = group |
                             std::views::transform([](const auto& pair) { return pair.second; }) |
                             std::ranges::to<std::vector>();
            map.emplace(kind, std::move(data_list));
        }

        return map;
    }
} // namespace enishi::platform