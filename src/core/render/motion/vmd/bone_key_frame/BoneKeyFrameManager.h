#pragma once
#include <vector>
#include <optional>
#include <DirectXMath.h>
#include "BoneKeyFrame.h"

constexpr uint32_t VMD_FPS = 30;
constexpr uint32_t FRAME_TIME = 1'000'000 / VMD_FPS; // 1フレームの時間

class BoneKeyFrameManager {
private:
	uint32_t key_frame_index;
	uint32_t current_key_frame;
	const std::vector<BoneKeyFrame> key_frame_list;

public:

	BoneKeyFrameManager(const std::vector<BoneKeyFrame>& key_frame_list);

	static BoneKeyFrameManager make(std::vector<BoneKeyFrame>&& key_frame_list);

	void set_frame(const uint32_t& frame);

	std::optional<BoneKeyFrame> get_previous_key_frame(void) const;
	std::optional<BoneKeyFrame> get_next_key_frame(void) const;
	std::optional<BoneKeyFrame> get_current_key_frame(void) const;
	std::optional<BoneKeyFrame> get_first_key_frame(void) const;
	std::optional<BoneKeyFrame> get_last_key_frame(void) const;

	// 線形補完済み
	DirectX::XMVECTOR get_rotate(const uint32_t elapsed_time) const;
	DirectX::XMVECTOR get_translate(const uint32_t elapsed_time) const;

	bool is_finished_motion(void) const;
};
