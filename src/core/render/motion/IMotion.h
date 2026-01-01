#pragma once
#include <cstdint>

class IMotion {
public:
	virtual ~IMotion(void) = default;

	//virtual void on_chaged_motion();

	virtual void update_motion(const int64_t delta_time) = 0;
};
