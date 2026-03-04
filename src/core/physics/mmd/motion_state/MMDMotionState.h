#pragma once

#include <LinearMath/btMotionState.h>

class MMDMotionState : public btMotionState {
public:
    virtual ~MMDMotionState(void) noexcept = default;

    virtual void reset(void) = 0;
    virtual void reflect_global_transform(void) = 0;
};
