#pragma once
#include "VMDFileStruct.h"
#include <filesystem>
#include <vector>

class BinaryReader;

class VMDLoader {
private:
	std::vector<VMDBoneKeyFrame> bone_key_frames;
	std::vector<VMDMorphKeyframe> morph_key_frames;
	std::vector<VMDIK> iks;

private:

	bool load_vmd_header(BinaryReader& binary_reader);
	bool load_vmd_bone_key_frame(BinaryReader& binary_reader);
	bool load_vmd_morph_key_frame(BinaryReader& binary_reader);
	bool load_vmd_camera(BinaryReader& binary_reader);
	bool load_vmd_light(BinaryReader& binary_reader);
	bool load_vmd_shadow(BinaryReader& binary_reader);
	bool load_vmd_ik(BinaryReader& binary_reader);

public:
	const std::filesystem::path path;

	explicit VMDLoader(const std::filesystem::path& path) noexcept;

	bool load_vmd(void);

	const std::vector<VMDBoneKeyFrame>& get_bone_key_frames(void) const noexcept;
	const std::vector<VMDMorphKeyframe>& get_morph_key_frames(void) const noexcept;
	const std::vector<VMDIK>& get_iks(void) const noexcept;
};
