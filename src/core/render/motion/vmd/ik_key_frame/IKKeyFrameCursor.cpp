#include "IKKeyFrameCursor.h"

IKKeyFrameCursor::IKKeyFrameCursor(
    const std::shared_ptr<KeyFrameTimer>& frame_manager,
    std::vector<IKKeyFrame>&& key_frame_list
) : KeyFrameCursor<IKKeyFrame>(
    frame_manager,
    IKKeyFrameCursor::sort(std::move(key_frame_list)),
    [](const IKKeyFrame& key_frame) {
        return 0;
    }
) {
}

bool IKKeyFrameCursor::is_apply_ik(void) {
    const auto& opt_previous = this->get_previous_key_frame();
    if(opt_previous.has_value()) {
        const auto& previous = opt_previous.value();
        return previous.show_flag;
    }

    const auto& opt_current = this->get_current_key_frame();
    if(opt_current.has_value()) {
        const auto& current = opt_current.value();
        return current.show_flag;
    }

    return false;
}

float IKKeyFrameCursor::progress(void) const {
    const auto& opt_pre_key_frame = this->get_previous_key_frame();
    const auto& opt_cur_key_frame = this->get_current_key_frame();

    if(!opt_cur_key_frame.has_value()) {
        return 1.0f;
    }
    const auto start_frame = opt_pre_key_frame.has_value() ? opt_pre_key_frame.value().index : 0;
    const auto end_frame = opt_cur_key_frame.value().index;

    return this->frame_manager->progress(
        start_frame,
        end_frame
    );
}