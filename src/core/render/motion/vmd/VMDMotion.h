#pragma once

#include "../IMotionHandler.h"
#include "bone_key_frame/BoneKeyFrameManager.h"
#include "bone_key_frame/BoneNode.h"
#include "ik_key_frame/IKKeyFrameManager.h"
#include <vector>

class IKSolver;
class IBoneAccessor;
struct Bones;

class VMDMotion : public IMotionHandler {
private:
	std::shared_ptr<IBoneAccessor> bone_accessor;
	std::unique_ptr<IKKeyFrameManager> ik;

	std::vector<BoneIndex> sorted_bones;
	std::unordered_map<BoneIndex, BoneKeyFrameManager> bone_key_frame_map;
	std::unordered_map<BoneIndex, BoneNode> bone_matrix_map;

	int64_t elapsed_time;
	uint32_t current_frame;
	uint32_t last_frame;

public:
	explicit VMDMotion(const std::shared_ptr<IBoneAccessor>& bone_accessor);

	bool init_motion(void) override;
	bool load_motion_file(const std::filesystem::path& path) override;
	void update_motion(const int64_t delta_time) override;
};
