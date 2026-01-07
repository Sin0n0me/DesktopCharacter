#pragma once
#include "../../motion/vmd/bone_key_frame/BoneNode.h"
#include "../bone/Bone.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <optional>

struct PMDIK;

class IKSolver {
private:
	const std::shared_ptr<const PMDBoneMap> bone_map;
	std::vector<std::vector<BoneIndex>> childern_tree;
	std::vector<PMDIK> iks;
	std::unordered_map<BoneIndex, uint32_t> ik_map; // first: frame

	void update_children_global(const uint16_t& root, std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map) const;

	bool solve_ik_bone(const PMDIK& ik, const uint16_t& bone_index, std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map) const;
public:

	explicit IKSolver(const std::shared_ptr<const PMDBoneMap>& bone_map) noexcept;

	void set_pmd_ik(const std::vector<PMDIK>&& iks);

	std::optional<PMDIK> get_ik(const BoneIndex& bone_index)const;

	void apply_ik(const PMDIK& ik, std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map) const;
};
