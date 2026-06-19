#include "model_loader.h"
#include "pmd/pmd_model_loader.h"
#include <foundation/log/logger.h>
#include <foundation/str/to_utf8.h>

namespace enishi::assets_system {
    foundation::EngineResult<ModelData, IOError> ModelLoader::load(
        const std::filesystem::path& path) {
        if (!path.has_extension()) {
            // return ;
        }

        if (PMDModelLoader::is_supported_extension(path)) {
            const auto pmd_data = PMDModelLoader::load(path);
            if (pmd_data.is_err()) {
                return pmd_data.error();
            }
        }

        return foundation::EngineResult<ModelData, IOError>();
    }

    ModelData ModelLoader::to_model_data_from_pmd(const PMDData& data) {
        const std::string sjis_name(data.model_name.data(), data.model_name.size());
        const auto utf8_name = foundation::sjis_to_utf8(sjis_name);
        if (utf8_name.is_err()) {
            foundation::Logger::warning("utf8に変換できない文字が含まれています");
        }

        data.vertices;

        return ModelData{
            .name = utf8_name.value_or(sjis_name),
        };
    }
} // namespace enishi::assets_system