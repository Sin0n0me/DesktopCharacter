#pragma once
#include "../errors/errors.h"
#include "../utility/binary_reader.h"
#include <engine_types/assets/shader/shader_data.h>
#include <filesystem>
#include <foundation/option/option.h>
#include <foundation/result/result.h>
#include <foundation/str/str.h>

namespace enishi::assets_system {
    class ShaderLoader {
      public:
        [[nodiscard]]
        static foundation::Result<types::ShaderData, AssetError> load(
            const std::filesystem::path& path) noexcept;

      private:
        [[nodiscard]]
        static foundation::Result<types::ShaderData, AssetError> load_spir_v(
            BinaryReader& reader) noexcept;
    };
} // namespace enishi::assets_system
