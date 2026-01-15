#include "../../../model/pmd/bone/Bone.h"
#include "../../../model/pmd/ik/IKSolver.h"
#include "IKKeyFrameManager.h"

IKKeyFrameManager::IKKeyFrameManager(const std::shared_ptr<const IKSolver>& ik_soulver)
    : ik_solver(ik_soulver) {
    this->previous_frame = 0;
}

void IKKeyFrameManager::set_frame(const uint32_t& frame) {
    this->previous_frame = frame;

    const auto& iter = this->ik_frame_map.find(frame);
    if(iter == this->ik_frame_map.end()) {
        return;
    }

    const auto& bone_list = iter->second;
    std::vector<BoneIndex> delete_keys;
    for(const auto& ik_profile : bone_list) {
        if(ik_profile.show_flag) {
            this->apply_bones.emplace(ik_profile.index);
        } else {
            delete_keys.push_back(ik_profile.index);
        }
    }

    for(const auto& key : delete_keys) {
        for(auto iter = this->apply_bones.begin(); iter != this->apply_bones.end(); ++iter) {
            if(*iter == key) {
                this->apply_bones.erase(iter);
            }
        }
    }
}

void IKKeyFrameManager::apply_ik(
    std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map,
    const uint32_t& frame
) {
    if(this->previous_frame > frame) {
        // 前回より小さい場合は何がIK適用すべきかわからないので
        // 一度リセットしてそのフレームまでにオンになっているIKのフラグを辿る
        this->apply_bones.clear();
        for(uint32_t i = 0; i < frame + 1; ++i) {
            this->set_frame(i);
        }
    } else {
        // フレームが大きく飛んだ場合のために差分を適用していく
        for(uint32_t i = this->previous_frame; i < frame + 1; ++i) {
            this->set_frame(i);
        }
    }

    // IK
    for(const auto& bone_index : this->apply_bones) {
        this->ik_solver->apply_ik(bone_index, bone_matrix_map);
    }
}

bool IKKeyFrameManager::resolve_bones(const std::vector<VMDIK>& iks, const std::unordered_map<std::string, BoneIndex>& bone_name_map) {
    for(const auto& ik : iks) {
        for(const auto& ik_info : ik.ik_infos) {
            const auto& iter = bone_name_map.find(ik_info.name);
            if(iter == bone_name_map.end()) {
                // TODO: log
                continue;
            }

            if(ik_info.flag > 1) {
                // TODO: log
            }

            this->ik_frame_map[ik.frame].emplace_back(
                IKProfile{
                    .index = iter->second,
                    .show_flag = ik_info.flag == 1
                }
            );
        }
    }

    return true;
}