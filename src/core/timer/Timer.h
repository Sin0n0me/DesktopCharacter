#pragma once
class Timer {
	void reset(void);

	float get_delta_time(void) const;

	void set_fps();
};
