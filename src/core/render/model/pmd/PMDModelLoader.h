#pragma once
#include "../bone/Bone.h"
#include "PMDFileStruct.h"
#include <filesystem>
#include <vector>

class BinaryReader;

class PMDModelLoader {
private:
	std::vector<PMDVertex> vertices;
	std::vector<BoneIndex> indices;
	std::vector<PMDMaterial> materials;
	std::vector<PMDMorph> morphs;
	std::vector<PMDBone> bones;
	std::vector<PMDIK> iks;

private:
	bool load_pmd_header(BinaryReader& file);
	bool load_pmd_vertices(BinaryReader& file);
	bool load_pmd_indices(BinaryReader& file);
	bool load_pmd_materials(BinaryReader& file);
	bool load_pmd_bones(BinaryReader& file);
	bool load_pmd_iks(BinaryReader& file);
	bool load_pmd_morph(BinaryReader& file);

public:
	const std::filesystem::path path;

	explicit PMDModelLoader(const std::filesystem::path& path) noexcept;

	bool load_pmd(void);

	const std::vector<PMDVertex>& get_vertices(void) const noexcept;
	const std::vector<BoneIndex>& get_indices(void) const noexcept;
	const std::vector<PMDMaterial>& get_materials(void) const noexcept;
	const std::vector<PMDMorph>& get_morphs(void) const noexcept;
	const std::vector<PMDBone>& get_bones(void) const noexcept;
	const std::vector<PMDIK>& get_iks(void) const noexcept;
};
