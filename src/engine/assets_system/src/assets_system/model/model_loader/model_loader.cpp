#include "model_loader.h"
#include "../bone/bone_data.h"
#include "../bone_resolver.h"
#include "pmd/pmd_model_loader.h"
#include "pmd/pmd_to_model_data.h"

namespace enishi::assets_system {
    foundation::Result<types::ModelData, IOError> ModelLoader::load(
        const std::filesystem::path& path) {
        if (!path.has_extension()) {
            // return ;
        }

        if (PMDModelLoader::is_supported_extension(path)) {
            const auto pmd_data = PMDModelLoader::load(path);
            if (pmd_data.is_err()) {
                return pmd_data.error();
            }

            const auto model_data = PMDToModelData::to_model_data(*pmd_data.value().get());
            if (model_data.is_err()) {
                // return model_data.error();
            }

            return model_data.value();
        }

        // 仮
        return foundation::Error(IOError::BrokenStream);
    }
} // namespace enishi::assets_system