#pragma once
#include "../pmd/bone/Bone.h"
#include "PMDFileStruct.h"
#include <filesystem>
#include <memory>
#include <vector>

class BinaryReader;

class PMDModelLoader {
private:
    std::shared_ptr<PMDVertices> vertices;
    std::shared_ptr<PMDIndices> indices;
    std::shared_ptr<PMDMaterials> materials;
    std::shared_ptr<PMDBones> bones;
    std::shared_ptr<PMDIKs> iks;
    std::shared_ptr<PMDMorphs> morphs;
    std::shared_ptr<PMDDisplayMorphs> display_morphs;
    std::shared_ptr<PMDBoneFrameNames> bone_frame_name;
    std::shared_ptr<PMDDisplayBones> display_bones;
    std::shared_ptr<PMDEnglishDictionary> english_dictionary;
    std::shared_ptr<PMDToonTexture> toon_textures;
    std::shared_ptr<PMDRigidBodies> rigid_bodies;
    std::shared_ptr<PMDPhysicsJoints> physics_joints;

private:
    bool load_pmd_header(BinaryReader& binary_reader);
    bool load_pmd_vertices(BinaryReader& binary_reader);
    bool load_pmd_indices(BinaryReader& binary_reader);
    bool load_pmd_materials(BinaryReader& binary_reader);
    bool load_pmd_bones(BinaryReader& binary_reader);
    bool load_pmd_iks(BinaryReader& binary_reader);
    bool load_pmd_morphs(BinaryReader& binary_reader);
    bool load_pmd_display_morphs(BinaryReader& binary_reader);
    bool load_pmd_bone_frame_names(BinaryReader& binary_reader);
    bool load_pmd_display_bones(BinaryReader& binary_reader);
    bool load_pmd_english_dictionary(BinaryReader& binary_reader);
    bool load_pmd_toon_textures(BinaryReader& binary_reader);
    bool load_pmd_rigid_bodies(BinaryReader& binary_reader);
    bool load_pmd_physics_joints(BinaryReader& binary_reader);

public:
    const std::filesystem::path path;

    explicit PMDModelLoader(const std::filesystem::path& path) noexcept;

    bool load_pmd(void);

    std::shared_ptr<const PMDVertices> get_vertices(void) const noexcept;
    std::shared_ptr<const PMDIndices> get_indices(void) const noexcept;
    std::shared_ptr<const PMDMaterials> get_materials(void) const noexcept;
    std::shared_ptr<const PMDBones> get_bones(void) const noexcept;
    std::shared_ptr<const PMDIKs> get_iks(void) const noexcept;
    std::shared_ptr<const PMDMorphs> get_morphs(void) const noexcept;
    std::shared_ptr<const PMDDisplayMorphs> get_display_morphs(void) const noexcept;
    std::shared_ptr<const PMDBoneFrameNames> get_bone_frame_name(void) const noexcept;
    std::shared_ptr<const PMDDisplayBones> get_display_bones(void) const noexcept;
    std::shared_ptr<const PMDEnglishDictionary> get_english_dictionary(void) const noexcept;
    std::shared_ptr<const PMDToonTexture> get_toon_textures(void) const noexcept;
    std::shared_ptr<const PMDRigidBodies> get_rigid_bodies(void) const noexcept;
    std::shared_ptr<const PMDPhysicsJoints> get_physics_joints(void) const noexcept;
};
