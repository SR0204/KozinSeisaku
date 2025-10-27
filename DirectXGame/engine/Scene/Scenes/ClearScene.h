#pragma once
#include "../../Scene/SceneManager/Scene.h"
#include "KamataEngine.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;

class ClearScene : public Scene {
public:
	ClearScene();
	~ClearScene();

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	uint32_t ClearTextureHandle_ = 0;
	uint32_t ClearTextureHandle2_ = 0;
	KamataEngine::Sprite* pressSpaceSprite_ = nullptr;

	// ゲームオーバー用
	KamataEngine::Sprite*ClearShadow_ = nullptr;
	KamataEngine::Sprite*ClearEdge_ = nullptr;
	KamataEngine::Sprite*ClearMain_ = nullptr;

	// 背景
	uint32_t ClearBG_ = 0;
	KamataEngine::Sprite* ClearBgSprite_ = nullptr;

	// フェードアウト用
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	float fadeAlpha_;
	bool isFadingOut_;
	bool isFadingIn_;
	float blinkAlpha_;

	int frameCount_ = 0;

	int waitTimer_ = 0;
	int waitAfterFade_ = 0;
};
