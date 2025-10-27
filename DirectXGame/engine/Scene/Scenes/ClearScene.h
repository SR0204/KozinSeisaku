#pragma once
#include "2d/Sprite.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/ObjectColor.h"
#include "KamataEngine.h"
#include "engine/Scene/SceneManager/SceneManager.h"
#include "input/Input.h"

class ClearScene : public Scene {
public:
	ClearScene();
	~ClearScene();

	void Initialize(SceneManager* sceneManager);
	void Update();
	void Draw();

private:
	// -----------------------------
	// 共通
	// -----------------------------
	SceneManager* sceneManager_ = nullptr;
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	// -----------------------------
	// フェード用スプライト
	// -----------------------------
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	float fadeAlpha_ = 1.0f;
	bool isFadingOut_ = false;
	bool isFadingIn_ = true;
	bool waitAfterFade_ = false;
	int waitTimer_ = 0;

	// -----------------------------
	// フレーム管理
	// -----------------------------
	int frameCount_ = 0;

	// -----------------------------
	// ClearScene 3Dモデル
	// -----------------------------
	KamataEngine::Model* ClearModel_ = nullptr;
	KamataEngine::WorldTransform ClearWT_;
	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Camera* camera_;

	float blinkAlpha_ = 1.0f;

	// -----------------------------
	// ClearScene背景 2Dモデル
	// -----------------------------
	KamataEngine::Model* backgroundModel_[2];
	KamataEngine::WorldTransform backgroundWT_[2];
	float bgScrollSpeed_ = 1.0f; // 背景の流れる速さ
};