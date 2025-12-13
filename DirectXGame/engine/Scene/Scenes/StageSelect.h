#pragma once
#include "../SceneManager/Scene.h"
#include "3d/LightGroup.h"
#include "KamataEngine.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/Model.h>
#include <Audio/Audio.h>
#include <memory>


class SceneManager;

/// <summary>
/// ステージ選択画面を作るクラス
/// </summary>
class StageSelect : public Scene {
public:
	StageSelect();
	~StageSelect() override;

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

	void DrawImGui();

private:
	SceneManager* sceneManager_ = nullptr;

	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::ImGuiManager* ImGui_ = nullptr;

	// === 背景 ===
	std::unique_ptr<KamataEngine::Sprite> BackGround_[2];
	uint32_t bgTexHandle_ = 0;
	float bgScrollSpeed_ = 1.5f;

	// === ステージアイコン ===
	static const int kMaxStage = 3;

	std::unique_ptr<KamataEngine::Model> stageCubeModel_[kMaxStage];
	std::unique_ptr<KamataEngine::Model> stageTextModel_[kMaxStage];
	KamataEngine::WorldTransform stageTransform_[kMaxStage];

	bool isStageRotating_ = false;
	int rotateTimer_ = 0;

	// === カーソル ===
	std::unique_ptr<KamataEngine::Sprite> cursorSprite_;
	uint32_t cursorTexHandle_ = 0;
	int currentStage_ = 0;
	bool isDecide_ = false;

	// === フェード ===
	std::unique_ptr<KamataEngine::Sprite> fadeSprite_;
	uint32_t fadeTexHandle_ = 0;
	float fadeAlpha_ = 0.0f;
	bool isFadingOut_ = false;

	// === BGM ===
	int bgmHandle_ = -1;
	int bgmVoiceHandle_ = -1;
	float bgmVolume_ = 1.0f;

	// === 3Dモデル（飾り）===
	std::unique_ptr<KamataEngine::Model> model_;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	std::unique_ptr<KamataEngine::LightGroup> lightGroup_;

	// === 決定演出 ===
	int frameCount_ = 0;

	//==============デバッグ用================
	int debugStageIndex_ = 0;
};
