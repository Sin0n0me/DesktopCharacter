#include "../../../utility/BinaryReader.h"
#include "../../constant_buffer/Bones.h"
#include "../../model/ik/IKSolver.h"
#include "VMDMotion.h"
#include <algorithm>
#include <ranges>
#include "../../model/pmd/IBoneAccessor.h"
#include "VMDLoader.h"

VMDMotion::VMDMotion(const std::shared_ptr<IBoneAccessor>& bone_accessor) {
	this->bone_accessor = bone_accessor;
	this->elapsed_time = 0;
	this->current_frame = 0;
	this->last_frame = 0;
}

bool VMDMotion::init_motion(void) {
	return true;
}

bool VMDMotion::load_motion_file(const std::filesystem::path& path) {
	if(bool(this->ik)) {
		return true;
	}

	VMDLoader loader = VMDLoader(path);
	if(!loader.load_vmd()) {
		return false;
	}

	this->ik = std::make_unique<IKKeyFrameManager>(
		this->bone_accessor->get_ik_soulver()
	);

	std::unordered_map<BoneIndex, std::vector<BoneKeyFrame>> temp_map;

	for(const auto& bone_key_frame : loader.get_bone_key_frames()) {
		try {
			const auto& bone_index = this->bone_accessor->get_bone_index(bone_key_frame.bone_name);
			temp_map[bone_index].emplace_back(BoneKeyFrame::make(bone_key_frame));
		} catch(const std::exception&) {
			continue;
		}
	}

	// 変換
	for(auto& pair : temp_map) {
		const auto& bone_index = pair.first;

		this->bone_key_frame_map.emplace(
			bone_index,
			BoneKeyFrameManager::make(std::move(pair.second))
		);

		// 最終キーフレームを求める
		const auto& opt_last = this->bone_key_frame_map
			.at(bone_index)
			.get_last_key_frame();
		if(opt_last.has_value()) {
			const auto& last = opt_last.value().frame_index;
			this->last_frame = this->last_frame < last ? last : this->last_frame;
		}
	}

	const auto& keys_view = std::views::keys(temp_map);
	this->sorted_bones = std::vector<BoneIndex>(keys_view.begin(), keys_view.end());
	std::sort(this->sorted_bones.begin(), this->sorted_bones.end());

	this->ik->resolve_bones(
		loader.get_iks(),
		this->bone_accessor->get_bone_name_map()
	);

	return true;
}

void VMDMotion::update_motion(const int64_t delta_time) {
	this->elapsed_time += delta_time;

	// 経過時間のリセット
	if(FRAME_TIME < this->elapsed_time) {
		this->current_frame += 1;
		this->elapsed_time = 0;
	}

	// フレームのリセット
	if(this->last_frame < this->current_frame) {
		this->current_frame = 0;
	}

	// TODO: to GPU (ローカル行列作成以外)

	// ローカル行列作成
	for(const auto& bone_index : this->sorted_bones) {
		auto& matrix = this->bone_matrix_map[bone_index];
		auto& key_frame_list = this->bone_key_frame_map.at(bone_index);
		key_frame_list.set_frame(this->current_frame);

		const auto& opt_previous_key_frame = key_frame_list.get_previous_key_frame();
		const auto& opt_bone_key_frame = key_frame_list.get_current_key_frame();
		const auto& bone_key_frame = opt_bone_key_frame.value();
		const auto& bind_bone = this->bone_accessor->get_bone(bone_index);

		// ローカル行列作成
		const auto anim_translate = DirectX::XMMatrixTranslationFromVector(
			key_frame_list.get_translate(this->elapsed_time)
		);
		const DirectX::XMMATRIX translate = bind_bone.local * anim_translate;
		const DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationQuaternion(
			key_frame_list.get_rotate(this->elapsed_time)
		);

		matrix.local = rotate * translate;
	}

	// グローバル行列作成
	for(const auto& bone_index : this->sorted_bones) {
		auto& matrix = this->bone_matrix_map.at(bone_index);
		const auto& bind_bone = this->bone_accessor->get_bone(bone_index);
		const int parent_index = bind_bone.parent;
		if(parent_index < 0) {
			matrix.global = matrix.local;
		} else {
			const auto& parent_global = this->bone_matrix_map.at(parent_index).global;
			matrix.global = matrix.local * parent_global;
		}
	}

	// IK
	this->ik->apply_ik(this->bone_matrix_map, this->current_frame);

	// スキニング用の定数バッファ結果を格納
	for(const auto& bone_index : this->sorted_bones) {
		const auto& matrix = this->bone_matrix_map.at(bone_index);
		const auto& inverse = this->bone_accessor->get_bone(bone_index).inverse_bind;
		auto mutable_matrix = this->bone_accessor->get_mutable_bones();
		mutable_matrix->bone_matrices[bone_index] = DirectX::XMMatrixTranspose(inverse * matrix.global);
	}
}