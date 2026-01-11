#pragma once
#include "../../motion/vmd/bone_key_frame/BoneNode.h"
#include "../bone/Bone.h"
#include "../pmd/PMDFileStruct.h"
#include <cstdint>
#include <memory>
#include <vector>

class IKSolver {
private:
	const std::shared_ptr<const PMDBoneMap> bone_map;
	std::vector<std::vector<BoneIndex>> childern_tree;
	std::unordered_map<BoneIndex, PMDIK> ik_map; // first: index, second: ik

	void update_children_global(
		const uint16_t& root,
		std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map
	) const;

	bool solve_ik_bone(
		const PMDIK& ik,
		const uint16_t& bone_index,
		std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map
	) const;

public:

	explicit IKSolver(
		const std::vector<PMDIK>& iks,
		const std::shared_ptr<const PMDBoneMap>& bone_map
	) noexcept;

	void apply_ik(
		const BoneIndex& bone_index,
		std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map
	) const;
};
