#pragma once
#include "../../errors/errors.h"
#include "../../interface_asset_system.h"
#include "interface_model_loader.h"
#include "pmd/pmd_data.h"
#include <engine_types/assets/model/model_data.h>
#include <filesystem>
#include <foundation/result/result.h>
#include <vector>

namespace enishi::assets_system {
    class ModelLoader {
      private:
        std::unordered_map<foundation::UTF8, std::unique_ptr<IModelLoader>> loaders;

      public:
        ModelLoader(void);

        foundation::Result<types::ModelData, AssetError> load(const std::filesystem::path& path);

      private:
        static types::ModelData to_model_data_from_pmd(const PMDData& data);
    };
} // namespace enishi::assets_system