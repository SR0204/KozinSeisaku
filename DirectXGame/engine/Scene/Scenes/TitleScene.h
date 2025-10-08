#pragma once
#include "../../Scene/SceneManager/Scene.h"
#include "KamataEngine.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;

class TitleScene : public Scene {
public:
	TitleScene();
	~TitleScene() override;

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

private:
	// シーン管理
	SceneManager* sceneManager_ = nullptr;

	// DirectX関連
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;     // PRESS SPACE
	KamataEngine::Sprite* sprite2_ = nullptr;    // Title ロゴ
	KamataEngine::Sprite* BackGround_ = nullptr; // 背景

	// テクスチャハンドル
	uint32_t TitleTextureHandle_ = 0;
	uint32_t TitleTextureHandle2_ = 0;
	uint32_t TitleBackGroundTextureHandle_ = 0;

	// BGM
	int bgmHandle_ = -1;
	int bgmVoiceHandle_ = -1;
	float bgmVolume_ = 1.0f;
	bool isFadingOut_ = false;

	// 演出用
	bool isTitle_ = true;    // タイトルシーン中フラグ
	int frameCount_ = 0;     // アニメーション用フレームカウント
	float fadeAlpha_ = 0.0f; // 画面暗転のアルファ値
	int bounceTimer_ = 0;    // ロゴバウンド用タイマー

	// バウンド用
	float bounceAmplitude_;
	const float bounceDecay_ = 0.9f;
	bool isBounceFinished_ = false;

	// フェード用スプライト
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	uint32_t fadeTextureHandle_ = 0;
};