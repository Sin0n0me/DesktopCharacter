#pragma once
#include "../../../../timer/Timer.h"
#include "KeyFrame.h"
#include <list>

class IKeyframeLifecycleListener;

class FrameManager {
private:
    std::list<IKeyframeLifecycleListener*> update_list;
    DeltaTime elapsed_time;
    DeltaTime frame_time;
    KeyFrame current_frame;
    KeyFrame max_frame;
    float fps;
    bool is_loop;
    bool is_no_limit;

public:

    explicit FrameManager(
        const float fps,
        const KeyFrame& max_frame
    );

    void set_is_loop_flag(const bool flag);

    void set_fps(const float fps);

    void set_max_frame(const KeyFrame& max_frame);

    void set_frame(const KeyFrame& frame);

    void update(const DeltaTime& delta_time);

    // 0-1
    float progress(
        const KeyFrame& start_frame,
        const KeyFrame& end_frame
    ) const;

    KeyFrame get_current_frame(void) const noexcept;

    bool is_progress(void) const noexcept;

    bool is_finished(void) const noexcept;

    void join(IKeyframeLifecycleListener* const target);

    void leave(const IKeyframeLifecycleListener* target);
};
