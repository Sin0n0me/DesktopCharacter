#include "OBB.h"
#include "../utility/Utility.h"
#include "../utility/Algorithm.h"

OBB OBB::make_by_covariance_matrix(const std::vector<DirectX::XMFLOAT3>& positions) {
	const float positions_size = static_cast<float>(positions.size());

	// 重心を求める
	DirectX::XMVECTOR mean = DirectX::XMVectorZero();
	for(const auto& position : positions) {
		mean += position;
	}
	mean /= positions_size;

	// 共分散行列の作成
	DirectX::XMMATRIX matrix{};
	for(const auto& position : positions) {
		const DirectX::XMFLOAT3 df = position - mean;
		matrix.r[0].m128_f32[0] += df.x * df.x;
		matrix.r[0].m128_f32[1] += df.x * df.y;
		matrix.r[0].m128_f32[2] += df.x * df.z;
		matrix.r[1].m128_f32[0] += df.y * df.x;
		matrix.r[1].m128_f32[1] += df.y * df.y;
		matrix.r[1].m128_f32[2] += df.y * df.z;
		matrix.r[2].m128_f32[0] += df.z * df.x;
		matrix.r[2].m128_f32[1] += df.z * df.y;
		matrix.r[2].m128_f32[2] += df.z * df.z;
	}
	matrix *= 1.0f / positions_size;
	matrix.r[3].m128_f32[3] = 1.0f; // 元の行列は単位行列ではないので

	// ヤコビ法による固有ベクトル取得
	DirectX::XMMATRIX eigenvectors{};
	jacobi_eigen_decomposition(matrix, eigenvectors);

	return OBB::make(positions, mean, eigenvectors);
}

OBB OBB::make(
	const std::vector<DirectX::XMFLOAT3>& positions,
	const DirectX::XMVECTOR& mean,
	const DirectX::XMMATRIX& eigen_vectors
) {
	// OBBの作成
	OBB obb{};
	for(int i = 0; i < 3; ++i) {
		const DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(eigen_vectors.r[i]);
		DirectX::XMStoreFloat3(&obb.axis[i], axis);
	}

	// 軸空間でAABB
	const DirectX::XMVECTOR minV = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	const DirectX::XMVECTOR maxV = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);

	DirectX::XMVECTOR min_vec = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	DirectX::XMVECTOR max_vec = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);
	for(const auto& position : positions) {
		const auto& temp = (position - mean);
		const DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&temp);
		const DirectX::XMVECTOR local = DirectX::XMVectorSet(
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[0]))),
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[1]))),
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[2]))),
			0.0f
		);
		min_vec = DirectX::XMVectorMin(min_vec, local);
		max_vec = DirectX::XMVectorMax(max_vec, local);
	}

	const DirectX::XMVECTOR half = sub_float(max_vec, min_vec) * 0.5f;
	const DirectX::XMVECTOR offset = add_float(max_vec, min_vec) * 0.5f;

	DirectX::XMStoreFloat3(&obb.half_extent, half);
	const auto offset_mean = mean +
		obb.axis[0] * offset.m128_f32[0] +
		obb.axis[1] * offset.m128_f32[1] +
		obb.axis[2] * offset.m128_f32[2];

	// 中心補正
	DirectX::XMStoreFloat3(&obb.center, offset_mean);

	return obb;
}