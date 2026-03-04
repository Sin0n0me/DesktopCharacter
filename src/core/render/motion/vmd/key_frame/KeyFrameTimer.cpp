#include "../../../../timer/DeltaTime.h"
#include "IKeyframeLifecycleListener.h"
#include "KeyFrameTimer.h"

KeyFrameTimer::KeyFrameTimer(const float fps, const KeyFrame& max_frame) :
    max_frame(max_frame),
    frame_timer(new FrameTimer(fps)),
    is_no_limit(false),
    is_loop(true) {
}

void KeyFrameTimer::set_is_loop_flag(const bool flag) {
    this->is_loop = flag;
}

void KeyFrameTimer::set_fps(const float fps) {
    this->frame_timer->set_fps(fps);
}

void KeyFrameTimer::set_max_frame(const KeyFrame& max_frame) {
    this->max_frame = max_frame;
}

void KeyFrameTimer::set_frame(const KeyFrame& frame) {
    this->frame_timer->set_current_frame(
        static_cast<FrameTimer::Frame>(frame)
    );
}

void KeyFrameTimer::update(const DeltaTime& delta_time) {
    this->frame_timer->add_delta_time(delta_time);

    if(this->is_loop && this->is_finished()) {
        this->frame_timer->reset();
    }

    for(auto target : this->update_list) {
        target->on_update_key_frame();
    }
}

double KeyFrameTimer::progress(
    const KeyFrame& start_frame,
    const KeyFrame& end_frame
) const {
    return this->frame_timer->progress(
        static_cast<FrameTimer::Frame>(start_frame),
        static_cast<FrameTimer::Frame>(end_frame)
    );
}

KeyFrame KeyFrameTimer::get_current_frame(void) const noexcept {
    return static_cast<KeyFrame>(this->frame_timer->get_current_frame());
}

bool KeyFrameTimer::is_progress(void) const noexcept {
    return !this->is_finished();
}

bool KeyFrameTimer::is_finished(void) const noexcept {
    return this->max_frame < this->get_current_frame();
}

void KeyFrameTimer::join(IKeyframeLifecycleListener* const target) {
    this->update_list.push_back(target);
}

void KeyFrameTimer::leave(const IKeyframeLifecycleListener* target) {
    // 離脱するたびに最悪O(N)かかるが離脱は頻繁に呼ばれない解放処理でしか使わないので計算量は無視する
    for(auto iter = this->update_list.begin(); iter != this->update_list.end(); ++iter) {
        if(*iter == target) {
            this->update_list.erase(iter);
            return;
        }
    }
}