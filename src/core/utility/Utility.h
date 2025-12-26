#pragma once
#include <DirectXMath.h>

static inline void operator+=(DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

static inline void operator-=(DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
}

static inline void operator*=(DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
}

static inline void operator/=(DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
}

static inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x += v2.x;
	temp.y += v2.y;
	temp.z += v2.z;
	return temp;
}

static inline DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x -= v2.x;
	temp.y -= v2.y;
	temp.z -= v2.z;
	return temp;
}

static inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x *= v2.x;
	temp.y *= v2.y;
	temp.z *= v2.z;
	return temp;
}

static inline DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x /= v2.x;
	temp.y /= v2.y;
	temp.z /= v2.z;
	return temp;
}

//
// DirectX::XMVECTOR <- DirectX::XMFLOAT3
//

static inline void operator+=(DirectX::XMVECTOR& v1, const float& v2) {
	v1.m128_f32[0] += v2;
	v1.m128_f32[1] += v2;
	v1.m128_f32[2] += v2;
	v1.m128_f32[3] += v2;
}

static inline void operator-=(DirectX::XMVECTOR& v1, const float& v2) {
	v1.m128_f32[0] -= v2;
	v1.m128_f32[1] -= v2;
	v1.m128_f32[2] -= v2;
	v1.m128_f32[3] -= v2;
}

static inline void operator*=(DirectX::XMVECTOR& v1, const float& v2) {
	v1.m128_f32[0] *= v2;
	v1.m128_f32[1] *= v2;
	v1.m128_f32[2] *= v2;
	v1.m128_f32[3] *= v2;
}

static inline void operator/=(DirectX::XMVECTOR& v1, const float& v2) {
	v1.m128_f32[0] /= v2;
	v1.m128_f32[1] /= v2;
	v1.m128_f32[2] /= v2;
	v1.m128_f32[3] /= v2;
}

// DirectX::XMFLOAT3

static inline void operator+=(DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	v1.m128_f32[0] += v2.x;
	v1.m128_f32[1] += v2.y;
	v1.m128_f32[2] += v2.z;
}

static inline void operator-=(DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	v1.m128_f32[0] -= v2.x;
	v1.m128_f32[1] -= v2.y;
	v1.m128_f32[2] -= v2.z;
}

static inline void operator*=(DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	v1.m128_f32[0] *= v2.x;
	v1.m128_f32[1] *= v2.y;
	v1.m128_f32[2] *= v2.z;
}

static inline void operator/=(DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	v1.m128_f32[0] /= v2.x;
	v1.m128_f32[1] /= v2.y;
	v1.m128_f32[2] /= v2.z;
}

static inline DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& v1, const float& v2) {
	DirectX::XMVECTOR temp{v1};
	temp.m128_f32[0] *= v2;
	temp.m128_f32[1] *= v2;
	temp.m128_f32[2] *= v2;
	return temp;
}

static inline DirectX::XMVECTOR operator/(const DirectX::XMVECTOR& v1, const float& v2) {
	DirectX::XMVECTOR temp{v1};
	temp.m128_f32[0] /= v2;
	temp.m128_f32[1] /= v2;
	temp.m128_f32[2] /= v2;
	return temp;
}

static inline DirectX::XMVECTOR operator+(const DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMVECTOR temp{v1};
	temp.m128_f32[0] += v2.x;
	temp.m128_f32[1] += v2.y;
	temp.m128_f32[2] += v2.z;
	return temp;
}

static inline DirectX::XMVECTOR operator-(const DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMVECTOR temp{v1};
	temp.m128_f32[0] -= v2.x;
	temp.m128_f32[1] -= v2.y;
	temp.m128_f32[2] -= v2.z;
	return temp;
}

static inline DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMVECTOR temp{v1};
	temp.m128_f32[0] *= v2.x;
	temp.m128_f32[1] *= v2.y;
	temp.m128_f32[2] *= v2.z;
	return temp;
}

static inline DirectX::XMVECTOR operator/(const DirectX::XMVECTOR& v1, const DirectX::XMFLOAT3& v2) {
	DirectX::XMVECTOR temp{v1};
	temp.m128_f32[0] /= v2.x;
	temp.m128_f32[1] /= v2.y;
	temp.m128_f32[2] /= v2.z;
	return temp;
}

//
// DirectX::XMFLOAT3 <- DirectX::XMVECTOR
//

static inline void operator+=(DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	v1.x += v2.m128_f32[0];
	v1.y += v2.m128_f32[1];
	v1.z += v2.m128_f32[2];
}

static inline void operator-=(DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	v1.x -= v2.m128_f32[0];
	v1.y -= v2.m128_f32[1];
	v1.z -= v2.m128_f32[2];
}

static inline void operator*=(DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	v1.x *= v2.m128_f32[0];
	v1.y *= v2.m128_f32[1];
	v1.z *= v2.m128_f32[2];
}

static inline void operator/=(DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	v1.x /= v2.m128_f32[0];
	v1.y /= v2.m128_f32[1];
	v1.z /= v2.m128_f32[2];
}

static inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x += v2.m128_f32[0];
	temp.y += v2.m128_f32[1];
	temp.z += v2.m128_f32[2];
	return temp;
}

static inline DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x -= v2.m128_f32[0];
	temp.y -= v2.m128_f32[1];
	temp.z -= v2.m128_f32[2];
	return temp;
}

static inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x *= v2.m128_f32[0];
	temp.y *= v2.m128_f32[1];
	temp.z *= v2.m128_f32[2];
	return temp;
}

static inline DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& v1, const DirectX::XMVECTOR& v2) {
	DirectX::XMFLOAT3 temp{v1};
	temp.x /= v2.m128_f32[0];
	temp.y /= v2.m128_f32[1];
	temp.z /= v2.m128_f32[2];
	return temp;
}