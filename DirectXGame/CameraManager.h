// CameraManager.h
#pragma once
#include "CameraController.h"
#include "Player.h"
#include <3d/Camera.h>
#include <3d/DebugCamera.h>
#include <KamataEngine.h>

class CameraManager {
public:
	void Initialize(KamataEngine::Camera* mainCamera, Player* target);
	void Update();
	void TransferMatrix();

	void SetDebugCameraActive(bool isActive);
	bool IsDebugCameraActive() const;

	const Camera& GetViewProjection() const;

	void SetMovableArea(const Rect& area);

private:
	KamataEngine::Camera* camera_ = nullptr;
	CameraController* cameraController_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;

	bool isDebugCameraActive_ = false;
};
