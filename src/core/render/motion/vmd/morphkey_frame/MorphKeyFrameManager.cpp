#include "../../../../log/Logger.h"
#include "../../../../utility/Convert.h"
#include "../../../model/pmd/morph/IMorphAccessor.h"
#include "../key_frame/KeyFrameTimer.h"
#include "MorphKeyFrameManager.h"
#include <unordered_map>

MorphKeyFrameManager::MorphKeyFrameManager(
    const std::shared_ptr<IMorphAccessor>& morph_accessor,
    const std::shared_ptr<KeyFrameTimer>& frame_manager,
    const std::vector<VMDMorphKeyFrame>& morph_key_frames
) {
    this->morph_accessor = morph_accessor;
    const auto size = morph_accessor->get_morphs()->size();

    std::unordered_map<uint32_t, std::vector<MorphKeyFrame>> temp_map;
    for(const auto& key_frames : morph_key_frames) {
        const auto opt_index = morph_accessor->get_morph_index(
            key_frames.morph_name
        );

        if(!opt_index.has_value()) {
            Logger::warning(
                Logger::make_message(
                    u8"Morph: 解決できない不明なボーン名: ",
                    sjis_to_utf8(key_frames.morph_name).value_or(u8"<UTF8に変換できない文字が含まれています>")
                )
            );
            continue;
        }

        // 0はすべてのベースなので除外
        const auto index = opt_index.value();
        if(index == 0) {
            continue;
        }

        if(key_frames.weight < 0) {
            // TODO: log
            continue;
        }

        temp_map[index].emplace_back(
            MorphKeyFrame{key_frames}
        );
    }

    for(auto& [index, key_frames] : temp_map) {
        this->morph_key_frames.emplace(
            index,
            std::make_unique<MorphKeyFrameCursor>(
                frame_manager,
                std::move(key_frames)
            )
        );
    }
}

void MorphKeyFrameManager::apply_morph(void) {
    for(auto& [index, morph_key_frame] : this->morph_key_frames) {
        morph_key_frame->apply_offset(
            index,
            this->morph_accessor
        );
    }
}

KeyFrame MorphKeyFrameManager::get_last_frame(void) const noexcept {
    KeyFrame last_frame = 0;

    // 最終キーフレームを求める
    for(const auto& [_, cursor] : this->morph_key_frames) {
        const auto& opt_last = cursor->get_last_key_frame();
        if(opt_last.has_value()) {
            const auto& last = opt_last.value().frame;
            last_frame = last_frame < last ? last : last_frame;
        }
    }

    return last_frame;
}