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
}

void CameraManager::Update() {
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
	} else {
		cameraController_->Update();
		camera_->matView = cameraController_->GetViewProjection().matView;
		camera_->matProjection = cameraController_->GetViewProjection().matProjection;
	}
}

void CameraManager::TransferMatrix() { camera_->TransferMatrix(); }

void CameraManager::SetDebugCameraActive(bool isActive) { isDebugCameraActive_ = isActive; }

bool CameraManager::IsDebugCameraActive() const { return isDebugCameraActive_; }

const Camera& CameraManager::GetViewProjection() const { return *camera_; }
