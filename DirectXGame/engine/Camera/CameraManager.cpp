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
