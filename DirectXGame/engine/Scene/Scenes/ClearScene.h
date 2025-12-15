#pragma once
#include "../../Scene/SceneManager/Scene.h"
#include "KamataEngine.h"
#include "math/Vector2.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;

/// <summary>
/// クリアシーンを作るクラス
/// </summary>
class ClearScene : public Scene {
public:
	ClearScene();
	~ClearScene() override;

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

	/// <summary>
	/// スコア受け取り用
	/// </summary>
	/// <param name="score"></param>
	void SetFinalScore(int score);

	/// <summary>
	/// ImGui用
	/// </summary>
	void DrawImGui() override;

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
	uint32_t ClearTextureHandle_ = 0;
	KamataEngine::Model* ClearSceneHandle2_ = nullptr;

	uint32_t ClearBackGroundTextureHandle_ = 0;

	KamataEngine::Sprite* BackGround_[2];
	float bgScrollSpeed_ = 1.0f;   // スクロール速度
	KamataEngine::Vector2 bgSize_; // 背景のサイズ

	KamataEngine::Model* ClearModel_ = nullptr;
	KamataEngine::WorldTransform ClearTransform_;
	KamataEngine::Camera Camera_; // 3D描画に必要

	// BGM
	int bgmHandle_ = -1;
	int bgmVoiceHandle_ = -1;
	float bgmVolume_ = 1.0f;
	bool isFadingOut_ = false;

	// 演出用
	bool isClear_ = true;    // GameOverシーン中フラグ
	int frameCount_ = 0;     // アニメーション用フレームカウント
	float fadeAlpha_ = 0.0f; // 画面暗転のアルファ値
	int bounceTimer_ = 0;    // ロゴバウンド用タイマー
	int blinkTimer_ = 0;     // 点滅用
	float baseRotY = 0;      // ゆらゆらアニメーション用
	int swayTimer_ = 0;      // ゆらゆらタイマー

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

	//-----------------スコア関係--------------
	int finalScore_ = 0;                                  // 最終スコア
	std::array<KamataEngine::Sprite*, 10> numberSprites_; // スコアの画像
	int displayScore_ = 0;                                // カウントアップ表示するスコア
	int scoreAddSpeed_ = 1;                               // 1フレームで増える量

	float scoreAnimScale_ = 1.0f; // ポヨン拡大アニメ
	float scoreAnimSpeed_ = 0.15f;
	bool isScoreComplete_ = false;

	KamataEngine::Sprite* shineSprite_; // 光エフェクト
	float shineAlpha_ = 0.0f;
	float shineX_ = 0.0f;
};