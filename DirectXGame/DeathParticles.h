#pragma once

#include "MathUtilityForText.h"
#include <3d/Model.h>
#include <3d/Camera.h>
#include <3d/WorldTransform.h>
#include <array>
#include <numbers>
#include<KamataEngine.h>

class DeathParticles {

public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	// デスフラグのgeeter
	bool IsFinished() const { return isFinished_; }

private:
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 存続時間(消滅までの時間)<秒>
	static inline const float kDuration = 5.0f;

	// 移動の速さ
	static inline const float kSpeed = 0.05f;

	// 分割した1個分の角度
	static inline const float kAngleUnit = 2 * std::numbers::pi_v<float> / kNumParticles;

	// 終了フラグ
	bool isFinished_ = false;

	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;

	// 色の数値
	Vector4 color_;

	// 終了フラグ
	// bool finished_ = false;
};