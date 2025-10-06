#define NOMINMAX
#define NOMINMIN
#include "CameraController.h"
#include "../Player/Player.h"
#include <algorithm>

void CameraController::Initialize() {

	// ビュープロジェクションの初期化
	camera_.Initialize();
}

void CameraController::Update() {

	Vector3 targetVelocity = target_->GetVelocity();

	// プレイヤーの位置を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// --- 追従目標位置を計算 ---
	// プレイヤーの位置 + オフセット（例：少し上から見下ろすなど）
	Vector3 targetPos = targetWorldTransform.translation_ + targetOffset_;

	// --- 滑らかに追従 ---
	// Lerpを使ってカメラがゆっくり追いつく
	camera_.translation_ = Lerp(camera_.translation_, targetPos, kInterpolationRate);

	// --- 回転（プレイヤー方向を向く） ---
	// 横スクロールなら特に回転しない（固定でOK）
	camera_.rotation_ = {0.0f, 0.0f, 0.0f};

	// --- 移動範囲制限 ---
	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, movableArea_.top);

	// --- カメラ行列を更新 ---
	camera_.UpdateMatrix();
}

void CameraController::Reset() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}