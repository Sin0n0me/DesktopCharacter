#pragma once
#include "../../errors/errors.h"
#include "../../interface_asset_system.h"
#include "../model_data.h"
#include "pmd/pmd_data.h"
#include <filesystem>
#include <foundation/result/result.h>
#include <vector>

namespace enishi::assets_system {
    class ModelLoader {
      public:
        static foundation::EngineResult<ModelData, IOError> load(const std::filesystem::path& path);

      private:
        static ModelData to_model_data_from_pmd(const PMDData& data);
    };
} // namespace enishi::assets_system