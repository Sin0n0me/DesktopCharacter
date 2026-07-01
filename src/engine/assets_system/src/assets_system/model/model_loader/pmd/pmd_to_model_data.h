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
    static constexpr glm::vec3 MMD_KNEE_AXIS = glm::vec3(-1.0f, 0.0f, 0.0f); // X軸固定

    class PMDToModelData {
      public:
        static foundation::Result<types::ModelData, AssetError> to_model_data(const PMDData& data);

      private:
        [[nodiscard]] static foundation::Result<BoneResolver, AssetError> make_bone(
            const std::vector<PMDBone>& bones);

        [[nodiscard]] static std::vector<types::SkinningVertex> make_vertices(
            const std::vector<PMDVertex>& vertices);

        [[nodiscard]] static std::vector<std::uint16_t> make_indices(
            const std::vector<PMDVertexIndex>& indices);

        [[nodiscard]] static std::vector<types::IK> make_iks(
            const std::vector<PMDIK>& iks, const IBoneResolver* bone_resolver);

        //[[nodiscard]] static std::vector<types::IK> make_iks(const std::vector<PMDIK>& iks);
    };
} // namespace enishi::assets_system