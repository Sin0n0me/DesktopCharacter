#include "../../../../utility/Algorithm.h"
#include "../../../constant_buffer/Bones.h"
#include "../../../model/pmd/bone/BoneNode.h"
#include "../../../model/pmd/bone/IBoneAccessor.h"
#include "BoneKeyFrameManager.h"
#include <unordered_map>

BoneKeyFrameManager::BoneKeyFrameManager(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<FrameManager>& frame_manager,
    const std::vector<VMDBoneKeyFrame>& key_frame_list
) : bone_accessor(bone_accessor) {
    // VMDで動かすボーンを収集
    // ボーンごとに纏める
    std::unordered_map<BoneIndex, std::vector<BoneKeyFrame>> temp_map;
    for(const auto& bone_key_frame : key_frame_list) {
        try {
            const auto& bone_index = bone_accessor->get_bone_index(
                bone_key_frame.bone_name
            );
            temp_map[bone_index].emplace_back(BoneKeyFrame{bone_key_frame});
        } catch(const std::exception&) {
            // TODO: log
            continue;
        }
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
    const auto& bone_matricies = this->bone_accessor->get_mutable_bone_nodes();
    for(const auto& [bone_index, key_frame_cursor] : this->bone_key_frame_map) {
        auto& matrix = bone_matricies->at(bone_index);

        const auto& opt_previous_key_frame = key_frame_cursor->get_previous_key_frame();
        const auto& opt_bone_key_frame = key_frame_cursor->get_current_key_frame();
        const auto& bone_key_frame = opt_bone_key_frame.value();
        const auto& bind_bone = this->bone_accessor->get_bone(bone_index);

        // ローカル行列作成
        const auto anim_translate = DirectX::XMMatrixTranslationFromVector(
            key_frame_cursor->get_translate()
        );
        const DirectX::XMMATRIX translate = bind_bone.local * anim_translate;
        const DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationQuaternion(
            key_frame_cursor->get_rotate()
        );

        matrix.local = rotate * translate;
    }
}

void BoneKeyFrameManager::update_global_matricies(void) {
    // グローバル行列作成
    const auto& bone_matricies = this->bone_accessor->get_mutable_bone_nodes();
    for(const auto& [bone_index, key_frame_cursor] : this->bone_key_frame_map) {
        auto& matrix = bone_matricies->at(bone_index);
        const auto& bind_bone = this->bone_accessor->get_bone(bone_index);
        const int parent_index = bind_bone.parent;
        if(parent_index < 0) {
            matrix.global = matrix.local;
        } else {
            const auto& parent_global = bone_matricies->at(parent_index).global;
            matrix.global = matrix.local * parent_global;
        }
    }
}

void BoneKeyFrameManager::apply_skinning(void) {
    // スキニング用の定数バッファ結果を格納
    const auto bone_matricies = this->bone_accessor->get_mutable_bone_nodes();
    for(const auto& [bone_index, key_frame_cursor] : this->bone_key_frame_map) {
        const auto& matrix = bone_matricies->at(bone_index);
        const auto& inverse = this->bone_accessor->get_bone(bone_index).inverse_bind;
        auto mutable_matrix = this->bone_accessor->get_mutable_bones();
        mutable_matrix->bone_matrices[bone_index] = DirectX::XMMatrixTranspose(inverse * matrix.global);
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