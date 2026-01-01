#pragma once

namespace std::filesystem {
	class path;
}

class IMotionHandler {
public:
	virtual ~IMotionHandler(void) = default;

	virtual bool init_motion(void) = 0;

	virtual bool load_motion_file(const std::filesystem::path& path) = 0;

	virtual void update_motion(const int64_t delta_time) = 0;
};
