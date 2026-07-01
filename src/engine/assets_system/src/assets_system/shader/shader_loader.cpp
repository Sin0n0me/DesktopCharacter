#include "shader_loader.h"

namespace enishi::assets_system {
    constexpr std::uint32_t SPIR_V_HEADER = 0x07230203;
    constexpr char DXBC_HEADER[] = "DXBC";

    foundation::Result<types::ShaderData, AssetError> ShaderLoader::load(
        const std::filesystem::path& path) noexcept {
        auto reader = BinaryReader::make_reader(path);
        if (!reader.has_value()) {
            return reader.propagation(AssetError::IOError);
        }
        auto& binary_reader = reader.value();

        if (binary_reader.read_magic_number(SPIR_V_HEADER).is_ok()) {
            return ShaderLoader::load_spir_v(binary_reader);
        }
        if (binary_reader.read_magic_number_from_str(DXBC_HEADER).is_ok()) {
        }

        return foundation::Error(AssetError::NotFound);
    }

    foundation::Result<types::ShaderData, AssetError> ShaderLoader::load_spir_v(
        BinaryReader& reader) noexcept {
        auto result = reader.read_all();
        if (result.is_err()) {
            return result.propagation(AssetError::IOError);
        }

        return types::ShaderData{
            .binary_type = types::ShaderBinaryType::SPIR_V,
            .code = std::move(result.value()),
        };
    }
} // namespace enishi::assets_system