#include "Timer.h"

Timer::Timer(void) noexcept {
    this->reset();
}

void Timer::reset(void) noexcept {
    this->start = std::chrono::high_resolution_clock::now();
    this->frame_start = this->start;
}

void Timer::frame_reset(void) noexcept {
    this->frame_start = std::chrono::high_resolution_clock::now();
}

DeltaTime Timer::get_delta_time(void) const {
    const auto now = std::chrono::high_resolution_clock::now();
    return DeltaTime(now - this->frame_start);
}