#pragma once
#include "../../timer/Timer.h"

class IMotion {
public:
    virtual ~IMotion(void) = default;

    //virtual void on_chaged_motion();

    virtual void update_motion(const DeltaTime& delta_time) = 0;
};
