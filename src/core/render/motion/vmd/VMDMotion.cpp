#include "../../../utility/BinaryReader.h"
#include "../../constant_buffer/Bones.h"
#include "../../../utility/Utility.h"
#include "VMDFileStruct.h"
#include "VMDMotion.h"
#include <algorithm>
#include <filesystem>
#include <ranges>

constexpr uint32_t VMD_FPS = 30;
constexpr uint32_t FRAME_TIME = 1'000'000 / VMD_FPS; // 1フレームの時間

VMDMotion::VMDMotion(const std::shared_ptr<Bones>& bones, const std::shared_ptr<const PMDBoneMap>& bone_map) {
	this->bones = bones;
	this->bone_map = bone_map;
	this->elapsed_time = 0;
	this->current_frame = 0;
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
	binary_reader.read_to(&count);

	binary_reader.read_to_vec(this->bone_key_frame_list, count);

	// 正規化
	for(auto& bone_key_frame : this->bone_key_frame_list) {
		const auto normalize_1uaternion = DirectX::XMQuaternionNormalize(DirectX::XMVectorSet(
			bone_key_frame.rotation[0],
			bone_key_frame.rotation[1],
			bone_key_frame.rotation[2],
			bone_key_frame.rotation[3]
		));

		bone_key_frame.rotation[0] = normalize_1uaternion.m128_f32[0];
		bone_key_frame.rotation[1] = normalize_1uaternion.m128_f32[1];
		bone_key_frame.rotation[2] = normalize_1uaternion.m128_f32[2];
		bone_key_frame.rotation[3] = normalize_1uaternion.m128_f32[3];
	}

	return true;
}

bool VMDMotion::load_vmd_morph_key_frame(BinaryReader& binary_reader) {
	uint32_t count = 0;
	binary_reader.read_to(&count);

	this->morph_key_frame.resize(count);
	binary_reader.read_to_vec(this->morph_key_frame, count);

	return true;
}

bool VMDMotion::resolve_bones(const std::unordered_map<std::string, uint16_t>& bone_name_map) {
	for(int index = 0; index < this->bone_key_frame_list.size(); ++index) {
		const auto& bone_key_frame = this->bone_key_frame_list[index];
		const auto& iter = bone_name_map.find(bone_key_frame.bone_name);
		if(iter == bone_name_map.end()) {
			continue;
		}
		const auto& bone_index = iter->second;

		this->resolved_bone_key_frame.emplace(index, bone_index);
		this->bone_key_frame_map[index].emplace_back(bone_key_frame.frame_index);
	}

	// 変換
	const auto& views_keys = std::views::keys(this->bone_key_frame_map);
	this->key_frame_list = std::vector<uint32_t>(views_keys.begin(), views_keys.end());
	std::ranges::sort(this->key_frame_list); // 昇順にソート

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
	{
	}

	return true;
}

void VMDMotion::update_motion(const int64_t delta_time) {
	this->elapsed_time += delta_time;

	// 経過時間のリセット
	if(this->elapsed_time > FRAME_TIME) {
		this->current_frame += 1;
		this->elapsed_time = 0;
	}

	const auto frame = this->key_frame_list[this->frame_index];
	if(this->current_frame > frame) {
		// 連続で現在のフレームが大きい場合は一巡しているのでリセット
		if(this->current_frame > this->key_frame_list[this->frame_index]) {
			this->current_frame = 0;
			this->frame_index = 0;
		}
	}

	struct BoneDelta {
		DirectX::XMVECTOR translation;
		DirectX::XMVECTOR rotation;
	};
	BoneDelta result{};

	for(const auto& pair : this->bone_key_frame_map) {
		const auto& bone_index = pair.first;
		const auto& key_frame_list = pair.second;

		for(const auto& index : this->bone_key_frame_map.at(frame)) {
			const auto& bone_index = this->resolved_bone_key_frame.at(index);
			const auto& bone_key_frame = this->bone_key_frame_list.at(bone_index);
			const auto& base_bone = this->bone_map->at(bone_index);

			// 線形補完
			result.translation = DirectX::XMVectorSet(
				bone_key_frame.translation[0],
				bone_key_frame.translation[1],
				bone_key_frame.translation[2],
				0.0f
			);
			result.rotation = DirectX::XMVectorSet(
				bone_key_frame.rotation[0],
				bone_key_frame.rotation[1],
				bone_key_frame.rotation[2],
				bone_key_frame.rotation[3]
			);

			// ローカル行列作成
			const DirectX::XMVECTOR bind_pos = DirectX::XMLoadFloat3(&base_bone.position);
			const DirectX::XMVECTOR final_pos = add_float(bind_pos, result.translation);
			const DirectX::XMMATRIX translate = DirectX::XMMatrixTranslationFromVector(final_pos);
			const DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationQuaternion(result.rotation);
			const DirectX::XMMATRIX local = translate * rotate;

			// グローバル
			DirectX::XMMATRIX global{};
			const int parent = base_bone.parent;
			if(parent < 0) {
				global = local;
			} else {
				global = this->bone_map->at(parent).global * local;
			}

			//
			this->bones->bone_matrices[bone_index] = global * base_bone.inverse_bind;
		}
	}
}