#pragma once
#include "../../../model/bone/Bone.h"
#include "../../../model/bone/IBoneNameResolver.h"
#include "../bone_key_frame/BoneNode.h"
#include "../VMDFileStruct.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

class IKSolver;

class IKKeyFrameManager : public IBoneNameResolver {
private:
	std::vector<VMDIK> vmd_iks; // temp
	std::shared_ptr<const IKSolver> ik_solver;
	std::unordered_map<uint32_t, std::vector<BoneIndex>> ik_frame_map; // first: frame
	std::set<BoneIndex> apply_bones;

public:
	explicit IKKeyFrameManager(const std::shared_ptr<const IKSolver>& ik_soulver);

	void set_vmd_iks(const std::vector<VMDIK>&& iks);

	void apply_ik(std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map, const uint32_t& frame);

	bool resolve_bones(const std::unordered_map<std::string, BoneIndex>& bone_name_map) override;
};
