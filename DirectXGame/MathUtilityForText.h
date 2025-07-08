#pragma once

#include <cmath>
#include <math/Matrix4x4.h>
#include <math/Vector3.h>
#include <numbers>

namespace KamataEngine {

// 行列の掛け算
inline Matrix4x4 MatrixMultiply(Matrix4x4& m1, Matrix4x4& m2) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row][col] = 0.0f;
			for (int k = 0; k < 4; k++) {
				result.m[row][col] += m1.m[row][k] * m2.m[k][col];
			}
		}
	}

	return result;
}

// アフィン行列生成
inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {
	(void)scale;
	// Y軸回転行列
	Matrix4x4 RotateMatY = {cosf(rot.y), 0, -sinf(rot.y), 0, 0, 1, 0, 0, sinf(rot.y), 0, cosf(rot.y), 0, 0, 0, 0, 1};

	// 平行移動行列
	Matrix4x4 TranslateMat = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, translate.x, translate.y, translate.z, 1};

	return MatrixMultiply(RotateMatY, TranslateMat);
}

// 線形補間
inline float Lerp(float x1, float x2, float t) { return (1.0f - t) * x1 + t * x2; }

// ベクトルの線形補間
inline Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3{Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t)}; }

// イージング補間
inline float EaseInOut(float x1, float x2, float t) {
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easedT);
}

// ========== Vector3 演算子オーバーロード ==========

// 加算
inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs) { return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }

// スカラー乗算（右）
inline Vector3 operator*(const Vector3& v, float s) { return Vector3{v.x * s, v.y * s, v.z * s}; }

// スカラー乗算（左）
inline Vector3 operator*(float s, const Vector3& v) { return v * s; }

// 代入付き加算
inline Vector3& operator+=(Vector3& lhs, const Vector3& rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

// 代入付きスカラー乗算
inline Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

} // namespace KamataEngine
