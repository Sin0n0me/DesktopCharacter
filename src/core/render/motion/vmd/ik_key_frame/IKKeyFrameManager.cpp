#include "../../../model/ik/IKSolver.h"
#include "../../../model/pmd/PMDFileStruct.h"
#include "IKKeyFrameManager.h"

IKKeyFrameManager::IKKeyFrameManager(const std::shared_ptr<const IKSolver>& ik_soulver)
	: ik_solver(ik_soulver) {
}

void IKKeyFrameManager::set_vmd_iks(const std::vector<VMDIK>&& iks) {
	this->vmd_iks = iks;
}

void IKKeyFrameManager::apply_ik(std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map, const uint32_t& frame) {
	const auto& iter = this->ik_frame_map.find(frame);
	if(iter != this->ik_frame_map.end()) {
		const auto& bone_list = iter->second;
		for(const auto& bone_index : bone_list) {
			this->apply_bones.emplace(bone_index);
		}
	}

	// IK
	for(const auto& bone_index : this->apply_bones) {
		const auto& opt_pmd_ik = this->ik_solver->get_ik(bone_index);
		const auto& pmd_ik = opt_pmd_ik.value();
		this->ik_solver->apply_ik(pmd_ik, bone_matrix_map);
	}
}

bool IKKeyFrameManager::resolve_bones(const std::unordered_map<std::string, BoneIndex>& bone_name_map) {
	for(const auto& ik : this->vmd_iks) {
		for(const auto& ik_info : ik.ik_infos) {
			const auto& iter = bone_name_map.find(ik_info.name);
			if(iter == bone_name_map.end()) {
				// TODO: log
				continue;
			}

			if(ik_info.flag == 0) {
				// TODO
			}

			const auto& bone_index = iter->second;
			this->ik_frame_map[ik.frame].emplace_back(bone_index);
		}
	}

	return true;
}