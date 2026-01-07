#pragma once
#include <DirectXMath.h>

// ヤコビ法による固有ベクトル取得
void jacobi_eigen_decomposition(
	DirectX::XMMATRIX& matrix,
	DirectX::XMMATRIX& eigenVectors
) noexcept;

float y_bezier(const float x, const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b, const uint8_t n);

// ベジェ曲線
float cubic_bezier(
	const float t,
	const float p0,
	const float p1,
	const float p2,
	const float p3
) noexcept;

DirectX::XMVECTOR slerp_quaternion(
	const DirectX::XMVECTOR& q0,
	const DirectX::XMVECTOR& q1,
	const float t
) noexcept;
