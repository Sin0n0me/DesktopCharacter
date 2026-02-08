#pragma once
#include "DeltaTime.h"
#include <chrono>

class Timer {
private:
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point frame_start;

public:
    explicit Timer(void) noexcept;

    void reset(void) noexcept;
    void frame_reset(void) noexcept;

    DeltaTime get_delta_time(void) const;
};
