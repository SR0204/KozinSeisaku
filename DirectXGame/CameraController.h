#pragma once
#include "MathUtilityForText.h"
#include <3d/Camera.h>
#include <KamataEngine.h>


class Player;

struct Rect {
	float left = 0.0f;   // 左端
	float right = 1.0f;  // 右端
	float bottom = 0.0f; // 下端
	float top = 1.0f;    // 上端
};

class CameraController {

public:
	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	const Camera& GetViewProjection() const { return camera_; }

	void SetMovableArea(Rect area) { movableArea_ = area; }

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	// 速度掛け算
	static inline const float kVelocityBias = 23.0f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect Margin = {-640.0, 640.0, -360.0, 360.0};

private:
	// ビュープロジェクション
	Camera camera_;

	// プレイヤー追従
	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差
	Vector3 targetOffset_ = {0, 0, -15.0f};

	// カメラの移動範囲
	Rect movableArea_ = {0, 100, 0, 100};

	// カメラの目標座標
	Vector3 targetcoordinates_;
};