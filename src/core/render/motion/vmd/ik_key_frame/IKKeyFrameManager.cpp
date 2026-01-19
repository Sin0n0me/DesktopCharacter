#include "../../../model/pmd/bone/Bone.h"
#include "../../../model/pmd/bone/IBoneAccessor.h"
#include "../../../model/pmd/ik/IKSolver.h"
#include "IKKeyFrameManager.h"

IKKeyFrameManager::IKKeyFrameManager(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<FrameManager>& frame_manager,
    const std::vector<VMDIK>& iks
) :
    ik_solver(bone_accessor->get_ik_soulver()),
    bone_accessor(bone_accessor) {
    const auto& name_map = bone_accessor->get_bone_name_map();

    std::unordered_map<uint32_t, std::vector<IKKeyFrame>> temp_map;
    for(const auto& ik : iks) {
        for(const auto& ik_info : ik.ik_infos) {
            const auto& iter = name_map.find(ik_info.name);
            if(iter == name_map.end()) {
                // TODO: log
                continue;
            }

            const bool frag = ik_info.flag == 1;
            if(ik_info.flag > 1) {
                // TODO: log
            }

            temp_map[iter->second].push_back(
                IKKeyFrame(ik.frame, frag)
            );
        }
    }

    for(auto& [bone_index, key_frames] : temp_map) {
        this->ik_frame_map.emplace(
            bone_index,
            std::make_unique<IKKeyFrameCursor>(
                frame_manager,
                std::move(key_frames)
            )
        );
    }
}

void IKKeyFrameManager::apply_ik(void) {
    for(auto& [bone_index, key_frame] : this->ik_frame_map) {
        if(key_frame->is_apply_ik()) {
            const auto bone_nodes = this->bone_accessor->get_mutable_bone_nodes();
            this->ik_solver->apply_ik(bone_index, *bone_nodes);
        }
    }
}