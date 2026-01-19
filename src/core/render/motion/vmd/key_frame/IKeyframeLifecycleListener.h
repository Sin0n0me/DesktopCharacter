#pragma once
class IKeyframeLifecycleListener {
public:

    virtual ~IKeyframeLifecycleListener(void) noexcept = default;

    virtual void on_update_key_frame(void) = 0;
};
