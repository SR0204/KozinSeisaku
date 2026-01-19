// CameraManager.h
#pragma once
#include "../../engine/Score/ScoreManager/ScoreManager.h"
#include "../Player/Player.h"
#include "CameraController.h"
#include <3d/Camera.h>
#include <3d/DebugCamera.h>
#include <KamataEngine.h>

/// <summary>
/// カメラコントローラーをまとめるマネージャークラス
/// </summary>
class CameraManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="mainCamera"></param>
	/// <param name="target"></param>
	void Initialize(KamataEngine::Camera* mainCamera, Player* target);

	void Update();

	/// <summary>
	/// カメラの行列
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// デバッグカメラ
	/// </summary>
	/// <param name="isActive"></param>
	void SetDebugCameraActive(bool isActive);
	bool IsDebugCameraActive() const;

	KamataEngine::Camera* GetViewProjection();

	void SetMovableArea(const Rect& area);

	/// <summary>
	/// カメラの揺れ
	/// </summary>
	/// <param name="duration"></param>
	/// <param name="strength"></param>
	void StartShake(float duration, float strength);
	void UpdateShake();

private:
	KamataEngine::Camera* camera_ = nullptr;
	CameraController* cameraController_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;

	bool isDebugCameraActive_ = false;

	float shakeTime_ = 0.0f;
	float shakeStrength_ = 0.0f;

	Vector3 baseCameraPos_;

	float jumpLookOffSet_ = 2.0f;
	float targetoffSet = 0.0f;

	float jumpCameraOffset_ = 0.0f;

	Player* target_ = nullptr;

	float comboStabilizeRate_ = 0.9f; // 0.8〜0.95
	Vector3 stabilizedPos_;
	bool isStabilizing_ = false;

	float apexBonus = 0.0f;

	float lookAheadX = 0.0f;
	float targetLookAheadX_ = 0.0f;
};
