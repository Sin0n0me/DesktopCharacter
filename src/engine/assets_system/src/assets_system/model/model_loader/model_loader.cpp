#include "model_loader.h"
#include "../bone/bone_data.h"
#include "../bone_resolver.h"
#include "pmd/pmd_model_loader.h"
#include "pmd/pmd_to_model_data.h"

namespace enishi::assets_system {
    ModelLoader::ModelLoader(void) {
        std::vector<std::unique_ptr<IModelLoader>> loaders;
        loaders.push_back(std::make_unique<PMDModelLoader>());

        for (auto& element : loaders) {
            if (bool(element)) {
                auto extension = element->get_supported_extension();
                this->loaders.emplace(std::move(extension), std::move(element));
            }
        }
    }

    foundation::Result<types::ModelData, AssetError> ModelLoader::load(
        const std::filesystem::path& path) {
        if (!path.has_extension()) {
            // return ;
        }

        const auto extension = path.extension().string<char>();
        auto iter = this->loaders.find(extension);
        if (iter == this->loaders.end()) {
            return foundation::Error(AssetError::NotFound, "対応していないファイル形式です’");
        }

        auto load_data = iter->second->load(path);
        if (load_data.is_err()) {
            return load_data.error();
        }
        auto& model_data = load_data.value();

        if (auto pmd_data = std::get_if<PMDData>(&model_data)) {
            const auto convert_data = PMDToModelData::to_model_data(*pmd_data);
            if (convert_data.is_err()) {
                // return model_data.error();
            }

            return convert_data.value();
        }

        // 仮
        return foundation::Error(AssetError::NotFound);
    }
} // namespace enishi::assets_system