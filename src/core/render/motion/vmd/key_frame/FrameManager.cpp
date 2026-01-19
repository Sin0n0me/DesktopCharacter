#include "FrameManager.h"
#include "IKeyframeLifecycleListener.h"
#include <algorithm>

FrameManager::FrameManager(const float fps, const KeyFrame& max_frame) {
    this->fps = fps;
    this->frame_time = static_cast<float>(Timer::BASE_SECOND) * 1.0f / fps;
    this->max_frame = max_frame;
    this->current_frame = 0;
    this->elapsed_time = 0;
    this->is_no_limit = false;
    this->is_loop = true;
}

void FrameManager::set_is_loop_flag(const bool flag) {
    this->is_loop = flag;
}

void FrameManager::set_fps(const float fps) {
    this->fps = fps;
    this->frame_time = static_cast<float>(Timer::BASE_SECOND) * 1.0f / fps;
}

void FrameManager::set_max_frame(const KeyFrame& max_frame) {
    this->max_frame = max_frame;
}

void FrameManager::set_frame(const KeyFrame& frame) {
    this->current_frame = frame;
    this->elapsed_time = 0;
}

void FrameManager::update(const DeltaTime& delta_time) {
    this->elapsed_time += delta_time;

    // 経過時間のリセット
    if(this->frame_time < this->elapsed_time) {
        this->elapsed_time = 0;
        this->current_frame += 1;

        // フレームのリセット
        if(this->is_loop && this->max_frame < this->current_frame) {
            this->current_frame = 0;
        }

        for(auto target : this->update_list) {
            target->on_update_key_frame();
        }
    }
}

float FrameManager::progress(
    const KeyFrame& start_frame,
    const KeyFrame& end_frame
) const {
    if(this->current_frame < start_frame) {
        return 0.0f;
    }
    if(end_frame < this->current_frame) {
        return 1.0f;
    }

    const float diff = end_frame - start_frame + 1;
    if(diff < 0) {
        return 0.0f;
    }
    const float offset = this->current_frame - start_frame;

    const float frame_count = static_cast<float>(this->frame_time);
    const float total_frame_time = diff * frame_count;
    const float current_frame_time = offset * frame_count + static_cast<float>(this->elapsed_time);

    const float progress = std::clamp(
        current_frame_time / total_frame_time,
        0.0f,
        1.0f
    );

    return progress;
}

KeyFrame FrameManager::get_current_frame(void) const noexcept {
    return this->current_frame;
}

bool FrameManager::is_progress(void) const noexcept {
    return this->current_frame != this->max_frame;
}

bool FrameManager::is_finished(void) const noexcept {
    return this->current_frame == this->max_frame;
}

void FrameManager::join(IKeyframeLifecycleListener* const target) {
    this->update_list.push_back(target);
}

void FrameManager::leave(const IKeyframeLifecycleListener* target) {
    // 離脱するたびに最悪O(N)かかるが離脱は頻繁に呼ばれない解放処理でしか使わないので計算量は無視する
    for(auto iter = this->update_list.begin(); iter != this->update_list.end(); ++iter) {
        if(*iter == target) {
            this->update_list.erase(iter);
            return;
        }
    }
}