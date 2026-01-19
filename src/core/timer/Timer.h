#pragma once
#include <chrono>
#include <cstdint>

using DeltaTime = int64_t;

class Timer {
public:
    using Base = std::chrono::microseconds;

public:
    static constexpr int64_t BASE_SECOND = Base::period::den;

private:
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point frame_start;

public:

    explicit Timer(void) noexcept;

    void reset(void) noexcept;
    void frame_reset(void) noexcept;

    DeltaTime get_delta_time(void) const;
};
