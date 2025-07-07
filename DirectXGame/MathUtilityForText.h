#pragma once
#include <3d\Camera.h>
#include <3d\Material.h>
#include <3d\WorldTransform.h>
#include <algorithm>
#include <base\DirectXCommon.h>
#include <base\StringUtility.h>
#include <base\TextureManager.h>
#include <cassert>
#include <d3dcompiler.h>
#include <format>
#include <fstream>
#include <math\MathUtility.h>
#include <numbers>
#include <sstream>
using namespace KamataEngine;

KamataEngine::Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

KamataEngine::Vector3& operator+=(Vector3& lhs, const Vector3& rhv);

// 行列の掛け算
KamataEngine::Matrix4x4 MatrixMultiply(Matrix4x4& m1, Matrix4x4& m2);

// EaseInOut関数
float EaseInOut(float x1, float x2, float t);

float Lerp(float x1, float x2, float t);

KamataEngine::Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 2項演算子のオーバーロード
// Vector3の足し算
const Vector3 operator+(const Vector3& v1, const Vector3& v2);

// 2項演算子のオーバーロード
// Vector3の掛け算
KamataEngine::Vector3& operator*=(Vector3& v, float s);