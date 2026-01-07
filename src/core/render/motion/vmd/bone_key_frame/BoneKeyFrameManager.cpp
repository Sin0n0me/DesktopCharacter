#include "../../../../utility/Algorithm.h"
#include "BoneKeyFrameManager.h"
#include <algorithm>

BoneKeyFrameManager::BoneKeyFrameManager(const std::vector<BoneKeyFrame>& key_frame_list) : key_frame_list(key_frame_list) {
	this->key_frame_index = 0;
	this->current_key_frame = 0;
}

BoneKeyFrameManager BoneKeyFrameManager::make(std::vector<BoneKeyFrame>&& key_frame_list) {
	// 昇順にソート
	std::sort(
		key_frame_list.begin(),
		key_frame_list.end(),
		[](const BoneKeyFrame& left, const BoneKeyFrame& right) {
			return left.frame_index < right.frame_index;
		}
	);
	return BoneKeyFrameManager(key_frame_list);
}

void BoneKeyFrameManager::set_frame(const uint32_t& frame) {
	if(this->key_frame_list.empty()) {
		return;
	};

	const auto& current_key_frame = this->key_frame_list.at(this->key_frame_index);
	if(current_key_frame.frame_index == frame) {
		return;
	}

	this->current_key_frame = frame;

	// 次のフレームへ
	if(current_key_frame.frame_index < frame) {
		if(this->get_next_key_frame().has_value()) {
			this->key_frame_index += 1;
		}
		return;
	}

	// 指定したキーフレームになるまで巻き戻す
	for(auto previous = this->get_previous_key_frame(); previous.has_value(); previous = this->get_previous_key_frame()) {
		if(frame < previous.value().frame_index) {
			this->key_frame_index -= 1;
		} else {
			break;
		}
	}
}

std::optional<BoneKeyFrame> BoneKeyFrameManager::get_previous_key_frame(void) const {
	if(this->key_frame_list.empty()) {
		return std::optional<BoneKeyFrame>();
	}

	if(this->key_frame_index == 0) {
		return std::optional<BoneKeyFrame>();
	}

	return this->key_frame_list.at(this->key_frame_index - 1);
}

std::optional<BoneKeyFrame> BoneKeyFrameManager::get_next_key_frame(void) const {
	if(this->key_frame_list.empty()) {
		return std::optional<BoneKeyFrame>();
	}

	// 末端と同じ場合は次はない扱い
	const auto& opt_current = this->get_current_key_frame();
	const auto& opt_last = this->get_last_key_frame();
	if(!opt_current.has_value() || !opt_last.has_value()) {
		return std::optional<BoneKeyFrame>();
	}
	const auto& current = opt_current.value();
	const auto& last = opt_last.value();
	if(current.frame_index == last.frame_index) {
		return std::optional<BoneKeyFrame>();
	}

	return this->key_frame_list.at(static_cast<uint64_t>(this->key_frame_index) + 1);
}

std::optional<BoneKeyFrame> BoneKeyFrameManager::get_current_key_frame(void) const {
	if(this->key_frame_list.empty()) {
		return std::optional<BoneKeyFrame>();
	}
	return this->key_frame_list.at(this->key_frame_index);
}

std::optional<BoneKeyFrame> BoneKeyFrameManager::get_first_key_frame(void) const {
	if(this->key_frame_list.empty()) {
		return std::optional<BoneKeyFrame>();
	}
	return this->key_frame_list.at(0);
}

std::optional<BoneKeyFrame> BoneKeyFrameManager::get_last_key_frame(void) const {
	if(this->key_frame_list.empty()) {
		return std::optional<BoneKeyFrame>();
	}
	return this->key_frame_list.at(this->key_frame_list.size() - 1);
}

DirectX::XMVECTOR BoneKeyFrameManager::get_rotate(const uint32_t elapsed_time) const {
	const auto& opt_previous = this->get_previous_key_frame();
	const auto& opt_current = this->get_current_key_frame();

	if(!opt_current.has_value()) {
		// TODO: log
		return DirectX::XMVectorZero();
	}

	const auto& current = opt_current.value();
	const auto& has_value = opt_previous.has_value();
	const auto& previous_index = has_value ? opt_previous.value().frame_index : 0;
	const auto& previous_rotate = has_value ? opt_previous.value().rotation : DirectX::XMQuaternionIdentity();
	const auto& diff = current.frame_index - previous_index;
	if(diff == 0) {
		return current.rotation;
	}
	const float current_frame = static_cast<float>((this->current_key_frame - previous_index) * FRAME_TIME);
	const float frame_count = static_cast<float>(diff * FRAME_TIME);
	const float frame_ratio = current_frame / frame_count;

	const DirectX::XMFLOAT2 p1 = DirectX::XMFLOAT2(static_cast<float>(current.interpolation[3]) / 127.0f, static_cast<float>(current.interpolation[7]) / 127.0f);
	const DirectX::XMFLOAT2 p2 = DirectX::XMFLOAT2(static_cast<float>(current.interpolation[11]) / 127.0f, static_cast<float>(current.interpolation[15]) / 127.0f);
	const float t = y_bezier(frame_ratio, p1, p2, 100); // TODO: config

	return slerp_quaternion(
		previous_rotate,
		current.rotation,
		t
	);
}

DirectX::XMVECTOR BoneKeyFrameManager::get_translate(const uint32_t elapsed_time) const {
	const auto& opt_previous = this->get_previous_key_frame();
	const auto& opt_current = this->get_current_key_frame();

	if(!opt_current.has_value()) {
		// TODO: log
		return DirectX::XMVectorZero();
	}

	const auto& current = opt_current.value();
	const auto& previous_index = opt_previous.has_value() ? opt_previous.value().frame_index : 0;
	const auto& previous_rotate = opt_previous.has_value() ? opt_previous.value().rotation : DirectX::XMQuaternionIdentity();
	const float diff = static_cast<float>(current.frame_index) - previous_index;
	if(diff == 0) {
		return current.translation;
	}

	return current.translation;
}

bool BoneKeyFrameManager::is_finished_motion(void) const {
	return !this->get_last_key_frame().has_value();
}