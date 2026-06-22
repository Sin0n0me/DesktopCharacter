#pragma once
#include "../errors/errors.h"
#include <cstdint>
#include <engine_types/assets/texture/texture_data.h>
#include <filesystem>
#include <foundation/result/result.h>
#include <vector>

namespace enishi::assets_system {
    class TextureLoader {
      public:
        static foundation::Result<types::TextureData, AssetError> load(
            const std::filesystem::path& path, const bool generate_mips, const bool force_srgb);

        static void generate_mip_chain(types::TextureData& data);

      private:
    };
} // namespace enishi::assets_system