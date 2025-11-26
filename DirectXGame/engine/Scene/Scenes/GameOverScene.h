#pragma once
#include "../../Scene/SceneManager/Scene.h"
#include "KamataEngine.h"
#include "math/Vector2.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;

/// <summary>
/// ゲームオーバーシーンを作るクラス
/// </summary>

class GameOverScene : public Scene {
public:
	GameOverScene();
	~GameOverScene() override;

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

private:
	// シーン管理
	SceneManager* sceneManager_ = nullptr;

	// DirectX関連
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;  // PRESS SPACE
	KamataEngine::Sprite* sprite2_ = nullptr; // GameOver ロゴ

	// テクスチャハンドル
	uint32_t GameOverTextureHandle_ = 0;
	KamataEngine::Model* GameOverSceneHandle2_ = nullptr;

	uint32_t GameOverBackGroundTextureHandle_ = 0;

	KamataEngine::Sprite* BackGround_[2];
	float bgScrollSpeed_ = 1.0f;   // スクロール速度
	KamataEngine::Vector2 bgSize_; // 背景のサイズ

	KamataEngine::Model* GameOverModel_ = nullptr;
	KamataEngine::WorldTransform GameOverTransform_;
	KamataEngine::Camera Camera_; // 3D描画に必要

	// BGM
	int bgmHandle_ = -1;
	int bgmVoiceHandle_ = -1;
	float bgmVolume_ = 1.0f;
	bool isFadingOut_ = false;

	// 演出用
	bool isGameOver_ = true; // GameOverシーン中フラグ
	int frameCount_ = 0;     // アニメーション用フレームカウント
	float fadeAlpha_ = 0.0f; // 画面暗転のアルファ値
	int bounceTimer_ = 0;    // ロゴバウンド用タイマー
	int blinkTimer_ = 0;     // 点滅用

	// バウンド用
	float bounceAmplitude_;
	const float bounceDecay_ = 0.9f;
	bool isBounceFinished_ = false;

	// フェード用スプライト
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	uint32_t fadeTextureHandle_ = 0;

	// ライト設定用
	KamataEngine::DirectionalLight light{};
	std::unique_ptr<KamataEngine::LightGroup> lightGroup_;
};