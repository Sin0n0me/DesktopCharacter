#include "../../../utility/BinaryReader.h"
#include "../../constant_buffer/Bones.h"
#include "../../model/ik/IKSolver.h"
#include "VMDFileStruct.h"
#include "VMDMotion.h"
#include <algorithm>
#include <ranges>
#include <filesystem>

VMDMotion::VMDMotion(
	const std::shared_ptr<Bones>& bones,
	const std::shared_ptr<const PMDBoneMap>& bone_map,
	const std::shared_ptr<const IKSolver>& ik_solver
) {
	this->bones = bones;
	this->bone_map = bone_map;
	this->elapsed_time = 0;
	this->current_frame = 0;
	this->last_frame = 0;
	this->ik = std::make_unique<IKKeyFrameManager>(ik_solver);
}

bool VMDMotion::load_vmd_header(BinaryReader& binary_reader) {
	VMDHeader header{};
	binary_reader.read_to(&header);

	if(std::strncmp(header.header, "Vocaloid Motion Data", 21) == 0) {
		return true;
	}
	if(std::strncmp(header.header, "Vocaloid Motion Data 0002", 26) == 0) {
		return true;
	}

	return false;
}

bool VMDMotion::load_vmd_bone_key_frame(BinaryReader& binary_reader) {
	uint32_t count = 0;
	if(!binary_reader.read_to(&count)) {
		return false;
	}

	if(!binary_reader.read_to_vec(this->bone_key_frame_list, count)) {
		return false;
	}

	return true;
}

bool VMDMotion::load_vmd_morph_key_frame(BinaryReader& binary_reader) {
	uint32_t count = 0;
	if(!binary_reader.read_to(&count)) {
		return false;
	}

	if(!binary_reader.read_to_vec(this->morph_key_frame, count)) {
		return false;
	}

	return true;
}

bool VMDMotion::load_vmd_camera(BinaryReader& binary_reader) {
	uint32_t count = 0;
	if(!binary_reader.read_to(&count)) {
		return false;
	}

	// 今のところ使用しない
	std::vector<VMDCameraKeyframe> camera;
	if(!binary_reader.read_to_vec(camera, count)) {
		return false;
	}

	return true;
}

bool VMDMotion::load_vmd_light(BinaryReader& binary_reader) {
	uint32_t count = 0;
	if(!binary_reader.read_to(&count)) {
		return false;
	}

	// 今のところ使用しない
	std::vector<VMDLight> camera;
	if(!binary_reader.read_to_vec(camera, count)) {
		return false;
	}

	return true;
}

bool VMDMotion::load_vmd_shadow(BinaryReader& binary_reader) {
	uint32_t count = 0;
	if(!binary_reader.read_to(&count)) {
		return false;
	}

	// 今のところ使用しない
	std::vector<VMDShadow> camera;
	if(!binary_reader.read_to_vec(camera, count)) {
		return false;
	}

	return true;
}

bool VMDMotion::load_vmd_ik(BinaryReader& binary_reader) {
	uint32_t count = 0;
	if(!binary_reader.read_to(&count)) {
		return false;
	}

	std::vector<VMDIK> iks(count);
	for(decltype(count) i = 0; i < count; i++) {
		auto& ik = iks[i];
		//
		const bool result = binary_reader.read(
			&ik,
			sizeof(VMDIK) - sizeof(VMDIK::ik_infos)
		);
		if(!result) {
			return false;
		}

		if(!binary_reader.read_to_vec(ik.ik_infos, ik.count)) {
			return false;
		}
	}
	this->ik->set_vmd_iks(std::move(iks));

	return true;
}

bool VMDMotion::resolve_bones(const std::unordered_map<std::string, BoneIndex>& bone_name_map) {
	std::unordered_map<BoneIndex, std::vector<BoneKeyFrame>> temp_map;

	for(const auto& bone_key_frame : this->bone_key_frame_list) {
		const auto& iter = bone_name_map.find(bone_key_frame.bone_name);
		if(iter == bone_name_map.end()) {
			// TODO: log
			continue;
		}
		const auto& bone_index = iter->second;
		temp_map[bone_index].emplace_back(BoneKeyFrame::make(bone_key_frame));
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

	this->ik->resolve_bones(bone_name_map);

	return true;
}

bool VMDMotion::init_motion(void) {
	return true;
}

bool VMDMotion::load_motion_file(const std::filesystem::path& path) {
	auto reader = BinaryReader::make_reader(path);
	if(!reader.has_value()) {
		return false;
	}
	auto& binary_reader = reader.value();

	// ヘッダ読み込み
	if(!this->load_vmd_header(binary_reader)) {
		return false;
	}

	// ボーンキーフレームの読み込み
	if(!this->load_vmd_bone_key_frame(binary_reader)) {
		return false;
	}

	// モーフの読み込み
	if(!this->load_vmd_morph_key_frame(binary_reader)) {
		return false;
	}

	// カメラの読み込み
	if(!this->load_vmd_camera(binary_reader)) {
		return false;
	}

	// ライトの読み込み
	if(!this->load_vmd_light(binary_reader)) {
		return false;
	}

	// 影の読み込み
	if(!this->load_vmd_shadow(binary_reader)) {
		return false;
	}

	// IKの読み込み
	if(!this->load_vmd_ik(binary_reader)) {
		return false;
	}

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
		const auto& bind_bone = this->bone_map->at(bone_index);

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
		const auto& bind_bone = this->bone_map->at(bone_index);
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
		const auto& inverse = this->bone_map->at(bone_index).inverse_bind;
		this->bones->bone_matrices[bone_index] = DirectX::XMMatrixTranspose(inverse * matrix.global);
	}
}