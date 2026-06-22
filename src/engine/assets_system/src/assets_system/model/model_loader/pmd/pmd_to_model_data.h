#pragma once
#include "../../../errors/errors.h"
#include "../../../interface_asset_system.h"
#include "../../bone_resolver.h"
#include "pmd_data.h"
#include <cstdint>
#include <engine_types/assets/model/model_data.h>
#include <filesystem>
#include <foundation/result/result.h>
#include <vector>

namespace enishi::assets_system {
    class PMDToModelData {
      public:
        static foundation::Result<types::ModelData, AssetError> to_model_data(const PMDData& data);

      private:
        static foundation::Result<BoneResolver, AssetError> make_bone(
            const std::vector<PMDBone>& bones);

        static std::vector<types::SkinningVertex> make_vertices(
            const std::vector<PMDVertex>& vertices);

        static std::vector<std::uint16_t> make_indices(const std::vector<PMDVertexIndex>& indices);
    };
} // namespace enishi::assets_system