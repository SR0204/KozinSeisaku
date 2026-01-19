#include "CameraManager.h"
using namespace KamataEngine;

void CameraManager::Initialize(KamataEngine::Camera* mainCamera, Player* target) {
	camera_ = mainCamera;

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(target);
	cameraController_->Reset();

	debugCamera_ = new DebugCamera(1280, 720);

	// 必要があればここで可動範囲を設定（別でもOK）
	Rect cameraArea = {12.0f, 100.0f - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	baseCameraPos_ = camera_->translation_;

	target_ = target;
}

void CameraManager::Update() {

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
	} else {
		cameraController_->Update();

		// ★ ここが重要！CameraController の結果を反映
		camera_->translation_ = cameraController_->GetViewProjection().translation_;
		camera_->rotation_ = cameraController_->GetViewProjection().rotation_;

		// ===== 前方予測カメラ（スムーズ版）=====
		if (target_) {
			targetLookAheadX_ = std::clamp(target_->GetMoveDirection() * 5.0f, -2.0f, 2.0f);
		} else {
			targetLookAheadX_ = 0.0f;
		}

		float rate = (std::abs(target_->GetMoveDirection()) < 0.01f) ? 0.25f  // 止まったら早く戻す
		                                                             : 0.15f; // 動いてる時はなめらか

		lookAheadX = lookAheadX * (1.0f - rate) + targetLookAheadX_ * rate;

		// 1回だけ適用
		camera_->translation_.x += lookAheadX;

		// ===== ジャンプ中は上を見る =====

		if (target_ && target_->IsJumping()) {
			targetoffSet = jumpLookOffSet_;
		}

		if (target_ && target_->IsJumpApex()) {
			apexBonus = 0.3f;
		}

		float finalTargetOffset = targetoffSet + apexBonus;

		// 補間は1回
		jumpCameraOffset_ = jumpCameraOffset_ * 0.85f + finalTargetOffset * 0.15f;

		float followY = cameraController_->GetViewProjection().translation_.y;

		// 落下中は追従弱め
		if (target_ && target_->IsJumping() && target_->GetVelocity().y < 0.0f) {
			camera_->translation_.y = camera_->translation_.y * 0.95f + (followY + jumpCameraOffset_) * 0.05f;
		} else {
			camera_->translation_.y = camera_->translation_.y * 0.8f + (followY + jumpCameraOffset_) * 0.2f;
		}

		// ===== コンボ中カメラ安定化 =====
		if (target_ && target_->IsComboActive()) {

			if (!isStabilizing_) {
				stabilizedPos_ = camera_->translation_;
				isStabilizing_ = true;
			}

			camera_->translation_.x = stabilizedPos_.x * comboStabilizeRate_ + camera_->translation_.x * (1.0f - comboStabilizeRate_);

			camera_->translation_.y = stabilizedPos_.y * comboStabilizeRate_ + camera_->translation_.y * (1.0f - comboStabilizeRate_);

		} else {
			isStabilizing_ = false;
		}

		camera_->UpdateMatrix();
	}

	// ====================================
	// シェイク処理（毎フレーム元位置を更新）
	// ====================================
	baseCameraPos_ = camera_->translation_; // ← これが重要！！

	if (shakeTime_ > 0.0f) {

		shakeTime_ -= 1.0f / 60.0f;

		float offsetX = ((rand() % 2001 - 1000) / 1000.0f) * shakeStrength_;
		float offsetY = ((rand() % 2001 - 1000) / 1000.0f) * shakeStrength_;

		// 毎フレーム正しい元位置からオフセット
		camera_->translation_.x = baseCameraPos_.x + offsetX;
		camera_->translation_.y = baseCameraPos_.y + offsetY;

		camera_->UpdateMatrix();
	}
}

void CameraManager::TransferMatrix() { camera_->TransferMatrix(); }

void CameraManager::SetDebugCameraActive(bool isActive) { isDebugCameraActive_ = isActive; }

bool CameraManager::IsDebugCameraActive() const { return isDebugCameraActive_; }

KamataEngine::Camera* CameraManager::GetViewProjection() { return camera_; }

void CameraManager::StartShake(float duration, float strength) {
	shakeTime_ = duration;
	shakeStrength_ = strength;
}
void CameraManager::UpdateShake() {}
