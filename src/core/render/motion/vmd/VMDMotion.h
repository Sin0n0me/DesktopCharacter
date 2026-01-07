#pragma once

#include "../../model/bone/IBoneNameResolver.h"
#include "../IMotionHandler.h"
#include "VMDFileStruct.h"
#include <vector>
#include "bone_key_frame/BoneKeyFrameManager.h"
#include "bone_key_frame/BoneNode.h"
#include "ik_key_frame/IKKeyFrameManager.h"

class BinaryReader;
class IKSolver;
struct Bones;

class VMDMotion : public IMotionHandler, public IBoneNameResolver {
private:
	std::vector<VMDBoneKeyFrame> bone_key_frame_list;
	std::vector<VMDMorphKeyframe> morph_key_frame;

	std::vector<BoneIndex> sorted_bones;
	std::unordered_map<BoneIndex, BoneKeyFrameManager> bone_key_frame_map;
	std::unordered_map<BoneIndex, BoneNode> bone_matrix_map;
	std::unique_ptr<IKKeyFrameManager> ik;
	std::shared_ptr<Bones> bones;
	std::shared_ptr<const PMDBoneMap> bone_map;

	int64_t elapsed_time;
	uint32_t current_frame;
	uint32_t last_frame;

private:

	bool load_vmd_header(BinaryReader& binary_reader);
	bool load_vmd_bone_key_frame(BinaryReader& binary_reader);
	bool load_vmd_morph_key_frame(BinaryReader& binary_reader);
	bool load_vmd_camera(BinaryReader& binary_reader);
	bool load_vmd_light(BinaryReader& binary_reader);
	bool load_vmd_shadow(BinaryReader& binary_reader);
	bool load_vmd_ik(BinaryReader& binary_reader);

public:
	VMDMotion(
		const std::shared_ptr<Bones>& bones,
		const std::shared_ptr<const PMDBoneMap>& bone_map,
		const std::shared_ptr<const IKSolver>& ik_solver
	);

	bool init_motion(void) override;
	bool load_motion_file(const std::filesystem::path& path) override;
	void update_motion(const int64_t delta_time) override;

	bool resolve_bones(const std::unordered_map<std::string, BoneIndex>& bone_name_map) override;
};
