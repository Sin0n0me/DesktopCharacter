#include "../../../../log/Logger.h"
#include "../../../../utility/Convert.h"
#include "../../../constant_buffer/Bones.h"
#include "../../../model/pmd/bone/BoneNode.h"
#include "../../../model/pmd/bone/IBoneAccessor.h"
#include "BoneKeyFrameManager.h"
#include <unordered_map>

BoneKeyFrameManager::BoneKeyFrameManager(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<KeyFrameTimer>& frame_manager,
    const std::vector<VMDBoneKeyFrame>& key_frame_list
) :
    bone_accessor(bone_accessor),
    root_nodes(bone_accessor->get_root_bones()) {
    // VMDで動かすボーンを収集
    // ボーンごとに纏める
    std::unordered_map<BoneIndex, std::vector<BoneKeyFrame>> temp_map;
    for(const auto& bone_key_frame : key_frame_list) {
        const auto& opt_bone_index = bone_accessor->get_bone_index(
            bone_key_frame.bone_name
        );

        if(!opt_bone_index.has_value()) {
            Logger::warning(
                Logger::make_message(
                    u8"Bone: 解決できない不明なボーン名: ",
                    sjis_to_utf8(bone_key_frame.bone_name).value_or(
                        Logger::make_message(
                            u8"<UTF8に変換できない文字が含まれています\n",
                            u8"元のボーン名: ",
                            std::u8string(
                                reinterpret_cast<const char8_t*>(bone_key_frame.bone_name),
                                reinterpret_cast<const char8_t*>(bone_key_frame.bone_name + sizeof(bone_key_frame.bone_name) - 1)
                            )
                        )
                    )
                )
            );
            continue;
        }

        const auto bone_index = opt_bone_index.value();
        temp_map[bone_index].emplace_back(BoneKeyFrame{bone_key_frame});
    }

    // 変換
    for(auto& [bone_index, bone_key_frames] : temp_map) {
        this->bone_key_frame_map.emplace(
            bone_index,
            std::make_unique<BoneKeyFrameCursor>(
                frame_manager,
                std::move(bone_key_frames)
            )
        );
    }
}

void BoneKeyFrameManager::update_local_matricies(void) {
    // ローカル行列作成
    for(const auto& [bone_index, key_frame_cursor] : this->bone_key_frame_map) {
        const auto& bone_node = this->bone_accessor->get_bone_node(bone_index);

        bone_node->set_translate(
            key_frame_cursor->get_translate()
        );
        bone_node->set_animation_rotate(
            key_frame_cursor->get_rotate()
        );

        bone_node->update_local();
    }
}

void BoneKeyFrameManager::update_global_matricies(void) {
    // グローバル行列作成
    for(const auto& bone_node : this->root_nodes) {
        bone_node->update_global();
    }
}

void BoneKeyFrameManager::apply_skinning(void) {
    // スキニング用の定数バッファ結果を格納
    for(const auto& bone_node : this->bone_accessor->get_all_bone_nodes()) {
        const auto& inverse = bone_node->bind_bone.global_inverse;
        bone_node->set_global(inverse * bone_node->get_global());
    }
}

KeyFrame BoneKeyFrameManager::get_last_frame(void) const noexcept {
    KeyFrame last_frame = 0;

    // 最終キーフレームを求める
    for(const auto& [_, cursor] : this->bone_key_frame_map) {
        const auto& opt_last = cursor->get_last_key_frame();
        if(opt_last.has_value()) {
            const auto& last = opt_last.value().frame;
            last_frame = last_frame < last ? last : last_frame;
        }
    }

    return last_frame;
}