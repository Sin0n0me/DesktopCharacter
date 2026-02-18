#pragma once
#include <chrono>
#include <cstdint>

class DeltaTime;

class FrameTimer {
public:
    using Frame = int64_t;
    using Resolution = std::chrono::microseconds;

public:
    static constexpr Resolution SECOND = Resolution(Resolution::period::den);

private:
    Resolution frame_time; // 1フレームの時間
    Resolution invalidation_limit; // 無効にする時間
    Resolution elapsed_time; // 経過時間
    Frame current_frame; // 現在のフレーム

public:
    explicit FrameTimer(const float fps);
    explicit FrameTimer(
        const float fps,
        const Resolution& invalidation_limit
    );

    void set_invalidation_limit(const Resolution& invalidation_limit);

    void add_delta_time(const DeltaTime& delta_time) noexcept;

    void reset(void) noexcept;
    void set_fps(const float fps) noexcept;
    void set_time(const Resolution& time) noexcept;
    void set_current_frame(const Frame& frame) noexcept;
    Frame get_current_frame(void) const noexcept;
    bool can_add_delta_time(const DeltaTime& delta_time) noexcept;

    double progress(
        const Frame& start_frame,
        const Frame& end_frame
    ) const noexcept;
};
