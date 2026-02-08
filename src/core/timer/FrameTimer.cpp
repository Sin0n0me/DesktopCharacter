#include "DeltaTime.h"
#include "FrameTimer.h"
#include <algorithm>

FrameTimer::FrameTimer(const float fps) :
    frame_time(std::chrono::duration_cast<FrameTimer::Resolution>(FrameTimer::SECOND / (fps == 0.0f ? 60.0f : fps))),
    invalidation_limit(FrameTimer::SECOND),
    current_frame(0),
    elapsed_time(Resolution::zero()) {
}

FrameTimer::FrameTimer(
    const float fps,
    const Resolution& invalidation_limit
) :
    frame_time(std::chrono::duration_cast<FrameTimer::Resolution>(FrameTimer::SECOND / (fps == 0.0f ? 60.0f : fps))),
    invalidation_limit(invalidation_limit > Resolution::zero() ? invalidation_limit : FrameTimer::SECOND),
    current_frame(0),
    elapsed_time(Resolution::zero()) {
}

void FrameTimer::set_invalidation_limit(const Resolution& invalidation_limit) {
    this->invalidation_limit = invalidation_limit;
}

void FrameTimer::add_delta_time(const DeltaTime& delta_time) noexcept {
    // 大きく時間が飛んだ場合は時間を進めない
    if(this->invalidation_limit < delta_time.delta_time) {
        return;
    }

    this->elapsed_time += delta_time.delta_time;

    // 1フレーム経過していなければ何もしない
    const auto& frame_time = this->frame_time;
    if(this->elapsed_time < frame_time) {
        return;
    }

    // 経過時間のリセット
    this->current_frame += (this->elapsed_time / frame_time);
    this->elapsed_time %= frame_time;
}

void FrameTimer::reset(void) noexcept {
    this->current_frame = 0;
    this->elapsed_time = Resolution::zero();
}

void FrameTimer::set_fps(const float fps) noexcept {
    if(fps == 0) {
        return;
    }

    this->frame_time = std::chrono::duration_cast<FrameTimer::Resolution>(FrameTimer::SECOND / fps);
}

void FrameTimer::set_time(const Resolution& time) noexcept {
    this->reset();
    this->current_frame += (this->elapsed_time / this->frame_time);
    this->elapsed_time %= this->frame_time;
}

void FrameTimer::set_current_frame(const FrameTimer::Frame& frame) noexcept {
    this->current_frame = frame;
    this->elapsed_time = FrameTimer::Resolution::zero();
}

FrameTimer::Frame FrameTimer::get_current_frame(void) const noexcept {
    return this->current_frame;
}

double FrameTimer::progress(
    const FrameTimer::Frame& start_frame,
    const FrameTimer::Frame& end_frame
) const noexcept {
    const auto current_frame = this->get_current_frame();
    if(current_frame < start_frame) {
        return 0.0f;
    }
    if(end_frame < current_frame) {
        return 1.0f;
    }

    const auto diff = end_frame - start_frame + 1;
    const auto offset = current_frame - start_frame;

    using DoubleResolution = std::chrono::duration<double, FrameTimer::Resolution::period>;
    const auto total_time = std::chrono::duration_cast<DoubleResolution>(diff * this->frame_time);
    const auto time_position = std::chrono::duration_cast<DoubleResolution>(
        this->frame_time * offset + this->elapsed_time
    );

    const auto progress = time_position / total_time;

    return std::clamp(progress, 0.0, 1.0);
}