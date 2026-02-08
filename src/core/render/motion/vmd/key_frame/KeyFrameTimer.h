#pragma once
#include "../../../../timer/FrameTimer.h"
#include "KeyFrame.h"
#include <list>
#include <memory>

class IKeyframeLifecycleListener;
class DeltaTime;

class KeyFrameTimer {
private:
    std::list<IKeyframeLifecycleListener*> update_list;
    std::unique_ptr<FrameTimer> frame_timer;
    KeyFrame max_frame;
    bool is_loop;
    bool is_no_limit;

public:

    explicit KeyFrameTimer(
        const float fps,
        const KeyFrame& max_frame
    );

    void set_is_loop_flag(const bool flag);

    void set_fps(const float fps);

    void set_max_frame(const KeyFrame& max_frame);

    void set_frame(const KeyFrame& frame);

    void update(const DeltaTime& delta_time);

    // 0-1
    double progress(
        const KeyFrame& start_frame,
        const KeyFrame& end_frame
    ) const;

    KeyFrame get_current_frame(void) const noexcept;

    bool is_progress(void) const noexcept;

    bool is_finished(void) const noexcept;

    void join(IKeyframeLifecycleListener* const target);

    void leave(const IKeyframeLifecycleListener* target);
};
