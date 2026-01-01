#pragma once
#include "../../model/bone/Bone.h"
#include "../IMotionHandler.h"
#include "VMDFileStruct.h"
#include <string>
#include <unordered_map>
#include <vector>

class BinaryReader;
struct Bones;

class VMDMotion : public IMotionHandler {
private:
	std::vector<VMDBoneKeyFrame> bone_key_frame_list;
	std::vector<VMDMorphKeyframe> morph_key_frame;

	std::unordered_map<int, BoneIndex> resolved_bone_key_frame;
	std::unordered_map<BoneIndex, std::vector<uint32_t>> bone_key_frame_map;
	std::vector<uint32_t> key_frame_list;
	std::shared_ptr<Bones> bones;
	std::shared_ptr<const PMDBoneMap> bone_map;

	int64_t elapsed_time;
	uint32_t current_frame;
	uint32_t frame_index;

private:

	bool load_vmd_header(BinaryReader& binary_reader);
	bool load_vmd_bone_key_frame(BinaryReader& binary_reader);
	bool load_vmd_morph_key_frame(BinaryReader& binary_reader);

public:
	VMDMotion(const std::shared_ptr<Bones>& bones, const std::shared_ptr<const PMDBoneMap>& bone_map);

	bool resolve_bones(const std::unordered_map<std::string, uint16_t>& bone_map);

	bool init_motion(void) override;
	bool load_motion_file(const std::filesystem::path& path) override;
	void update_motion(const int64_t delta_time) override;
};
