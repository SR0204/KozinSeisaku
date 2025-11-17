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

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	Vector3 targetPos = targetWorldTransform.translation_ + targetOffset_;

	// --- 初回だけは一気に追従（画面外に出さない） ---
	if (firstFrame_) {
		camera_.translation_ = targetPos;
		firstFrame_ = false;
	} else {
		// 通常時はLerpで追従
		camera_.translation_.x = Lerp(camera_.translation_.x, targetPos.x, kInterpolationRate);
		camera_.translation_.y = Lerp(camera_.translation_.y, targetPos.y, kInterpolationRate);
	}

	camera_.rotation_ = {0.0f, 0.0f, 0.0f};
	camera_.UpdateMatrix();
}


void CameraController::Reset() {

	// 追従対象のワールドトランスフォーム
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 初期化時は即座にプレイヤー位置へ移動（Lerpは使わない）
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;

	camera_.UpdateMatrix();
}