#include "../../../utility/Utility.h"
#include "../../model/pmd/PMDFileStruct.h"
#include "IKSolver.h"
#include <algorithm>
#include <cmath>
#include <DirectXMath.h>
#include <stack>

IKSolver::IKSolver(const std::shared_ptr<const PMDBoneMap>& bone_map) noexcept : bone_map(bone_map) {
}

void IKSolver::set_pmd_ik(const std::vector<PMDIK>&& iks) {
	this->iks = iks;
	for(int i = 0; i < this->iks.size(); ++i) {
		const auto& ik = this->iks[i];
		this->ik_map[ik.ik_bone] = i;
	}

	this->childern_tree.resize(this->bone_map->size());
	for(const auto& [index, data] : *this->bone_map) {
		if(data.parent < 0) {
			continue;
		}

		this->childern_tree[data.parent].emplace_back(index);
	}

	for(auto& iter : this->childern_tree) {
		std::sort(iter.begin(), iter.end(), std::greater<>());
	}
}

std::optional<PMDIK> IKSolver::get_ik(const BoneIndex& bone_index) const {
	const auto& iter = this->ik_map.find(bone_index);
	if(iter == this->ik_map.end()) {
		return std::optional<PMDIK>();
	}

	return this->iks.at(iter->second);
}

void IKSolver::apply_ik(const PMDIK& ik, std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map) const {
	if(ik.chain.empty()) {
		return;
	}

	for(int i = 0; i < ik.iterations; ++i) {
		for(const auto& bone_index : ik.chain) {
			if(!this->solve_ik_bone(ik, bone_index, bone_matrix_map)) {
				continue;
			}
			this->update_children_global(bone_index, bone_matrix_map);
		}
	}
}

// 再帰でもよかったかもしれない
void IKSolver::update_children_global(
	const uint16_t& root,
	std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map
) const {
	// 初期ヒープサイズによっては毎回メモリ確保が起こり重くなるので修正が必要かもしれない
	std::stack<BoneIndex> stack = std::stack<BoneIndex>();
	stack.push(root);

	while(!stack.empty()) {
		const auto bone_index = stack.top();
		stack.pop();

		const auto parent = this->bone_map->at(bone_index).parent;
		auto& bone_matrix = bone_matrix_map.at(bone_index);
		if(parent < 0) {
			bone_matrix.global = bone_matrix.local;
		} else {
			const auto& parent_global = bone_matrix_map.at(parent).global;
			bone_matrix.global = bone_matrix.local * parent_global;
		}

		for(const auto& child : this->childern_tree.at(bone_index)) {
			stack.push(child);
		}
	}
}

bool IKSolver::solve_ik_bone(const PMDIK& ik, const uint16_t& bone_index, std::unordered_map<BoneIndex, BoneNode>& bone_matrix_map) const {
	const DirectX::XMVECTOR bone_pos = bone_matrix_map.at(bone_index).global.r[3];
	const DirectX::XMVECTOR ik_pos = bone_matrix_map.at(ik.ik_bone).global.r[3];
	const DirectX::XMVECTOR target_pos = bone_matrix_map.at(ik.target_bone).global.r[3];

	const DirectX::XMVECTOR ik_vec = DirectX::XMVector3Normalize(
		sub_float(ik_pos, bone_pos)
	);
	const DirectX::XMVECTOR target_vec = DirectX::XMVector3Normalize(
		sub_float(target_pos, bone_pos)
	);
	const float dot = std::clamp(
		DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(target_vec, ik_vec)
		),
		-1.0f,
		1.0f
	);

	float angle = std::acos(dot);
	const float epsilon = 1e-7f;
	if(angle < epsilon) {
		return false;
	}
	angle = std::clamp(angle, -ik.limit, ik.limit);

	const DirectX::XMVECTOR cross = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(target_vec, ik_vec)
	);
	if(DirectX::XMVector3Equal(cross, DirectX::XMVectorZero())) {
		return false;
	}

	auto& matrix = bone_matrix_map.at(bone_index);
	if(bone_index == 5 || bone_index == 8) {
		const DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(
			cross * -1.0,
			angle
		);
		const DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationQuaternion(quat);

		matrix.rotate = quat;
		matrix.local = rotate * matrix.local;

		/*
		const DirectX::XMVECTOR quat_rot = DirectX::XMQuaternionRotationAxis(
			axis,
			angle2
		);
		const DirectX::XMMATRIX rot_mat = DirectX::XMMatrixRotationQuaternion(
			quat_rot
		);

		// ローカル行列に合成
		matrix.local = rot_mat * matrix.local;
		*/
	} else {
		const DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(
			cross,
			angle
		);
		const DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationQuaternion(quat);

		matrix.local = rotate * matrix.local;
	}

	return true;
}