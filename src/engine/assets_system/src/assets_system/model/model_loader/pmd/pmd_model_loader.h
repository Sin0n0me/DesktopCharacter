#pragma once
#include "../../../utility/binary_reader.h"
#include "pmd_data.h"
#include "pmd_file_struct.h"
#include <filesystem>
#include <memory>

namespace enishi::assets_system {
    class PMDModelLoader {
      private:
        using Result = IOReuslt<void>;

        [[nodiscard]] Result load_pmd(BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_header(BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_vertices(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_indices(BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_materials(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_bones(BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_iks(BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_morphs(BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_display_morphs(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_bone_frame_names(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_display_bones(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_english_dictionary(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_toon_textures(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_rigid_bodies(
            BinaryReader& binary_reader, PMDData* const pmd_data);
        [[nodiscard]] Result load_pmd_physics_joints(
            BinaryReader& binary_reader, PMDData* const pmd_data);

        explicit PMDModelLoader(void) = default;

      public:
        [[nodiscard]] static IOReuslt<std::unique_ptr<PMDData>> load(
            const std::filesystem::path& path) noexcept;

        [[nodiscard]] static bool is_supported_extension(
            const std::filesystem::path& path) noexcept;
    };
} // namespace enishi::assets_system
