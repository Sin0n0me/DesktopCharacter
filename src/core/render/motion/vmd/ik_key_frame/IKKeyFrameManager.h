#pragma once
#include "../../../model/bone/Bone.h"
#include "../bone_key_frame/BoneNode.h"
#include "../VMDFileStruct.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>

class IKSolver;

class IKKeyFrameManager {
private:
	std::shared_ptr<const IKSolver> ik_solver;
	std::unordered_map<uint32_t, std::vector<BoneIndex>> ik_frame_map; // first: frame
	std::set<BoneIndex> apply_bones;

public:
	explicit IKKeyFrameManager(const std::shared_ptr<const IKSolver>& ik_soulver);

	void apply_ik(
		std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map,
		const uint32_t& frame
	);

	bool resolve_bones(
		const std::vector<VMDIK>& iks,
		const std::unordered_map<std::string, BoneIndex>& bone_name_map
	);
};
