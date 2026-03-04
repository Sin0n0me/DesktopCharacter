#include "../../../../log/Logger.h"
#include "../../../../utility/Convert.h"
#include "../../../model/pmd/bone/IBoneAccessor.h"
#include "../../../model/pmd/ik/IKSolver.h"
#include "IKKeyFrameCursor.h"
#include "IKKeyFrameManager.h"

IKKeyFrameManager::IKKeyFrameManager(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<KeyFrameTimer>& frame_manager,
    const std::shared_ptr<IKSolver>& ik_solver,
    const std::vector<VMDIK>& iks
) :
    ik_solver(ik_solver),
    bone_accessor(bone_accessor),
    frame_manager(frame_manager),
    ik_frame_map(decltype(ik_frame_map)()) {
    std::unordered_map<uint32_t, std::vector<IKKeyFrame>> temp_map;
    for(const auto& ik : iks) {
        for(const auto& ik_info : ik.ik_infos) {
            const auto& opt_index = bone_accessor->get_bone_index(ik_info.name);
            if(!opt_index.has_value()) {
                Logger::warning(
                    Logger::make_message(
                        u8"IK: 解決できない不明なボーン名: ",
                        sjis_to_utf8(ik_info.name).value_or(u8"<UTF8に変換できない文字が含まれています>")
                    )
                );
                continue;
            }

            const auto& index = opt_index.value();

            const bool frag = ik_info.flag == 1;
            if(1 < ik_info.flag) {
                Logger::make_message(
                    u8"想定していないフラグ値です. Falseとして扱いました\n",
                    u8"読み込んだ値: ",
                    ik_info.flag,
                    u8"ボーン名: ",
                    sjis_to_utf8(ik_info.name).value_or(u8"<UTF8に変換できない文字が含まれています>")
                );
            }

            temp_map[index].push_back(
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
    if(this->frame_manager->get_current_frame() == 0) {
        // TODO: reset
    }

    for(const auto& [bone_index, key_frame] : this->ik_frame_map) {
        if(!key_frame->is_apply_ik()) {
            continue;
        }

        this->ik_solver->apply_ik(bone_index);
    }
}