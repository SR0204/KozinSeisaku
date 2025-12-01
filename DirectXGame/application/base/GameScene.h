#pragma once

#include "../../engine/Camera/CameraManager.h"
#include "../../engine/Enemy/EnemyManager.h"
#include "../../engine/Map/MapManager.h"
#include "../../engine/Map/SkyDome/SkyDome.h"
#include "../../engine/Particle/DeathParticles.h"
#include "../../engine/Player/Player.h"
#include "../../engine/Scene/SceneManager/PhaseManager.h"
#include "../../engine/Scene/SceneManager/SceneManager.h"
#include "../../engine/Score/Score.h"
#include "../../engine/Spawn/SpawnManager.h"
#include "../../engine/Time/Timer.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/DebugCamera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <audio/Audio.h>
#include <base/DirectXCommon.h>
#include <input/Input.h>
#include <vector>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public Scene {

public: // メンバ関数(引数）
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SceneManager* sceneManager) override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ゲームのフェーズ(型)
	enum class Phase {
		kTitle,
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
	};

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// デスフラグのgeeter
	bool IsFinished() const { return finished_; }

	// 時間表示用の関数
	void DrawTimeUI();

private: // メンバ変数（関数）
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	//--------------------マネージャー系統----------------------------//
	MapManager* mapManager_;

	EnemyManager* enemyManager_ = nullptr;

	PhaseManager* phaseManager_ = nullptr;

	CameraManager* cameraManager_ = nullptr;

	SceneManager* sceneManager_ = nullptr;
	SceneID nextScene_ = SceneID::None;

	//--------------------マネージャー系統----------------------------//

	//-----------------------3Dモデルの生成-----------------------//
	Model* model_ = nullptr;

	Model* EnemyModel_ = nullptr;

	Model* modelSkydome_ = nullptr;

	// プレイヤーモデル
	Model* modelPlayer_ = nullptr;
	// ブロックのモデルを読み込む
	Model* modelBlock_ = 0;
	// デスパーティクルモデル
	Model* deathParticleModel_ = nullptr;

	//-----------------------3Dモデルの生成-----------------------//

	// ビュープロジェクション生成
	Camera camera_;

	// プレイヤーの生成
	Player* player_ = nullptr;

	// 天球の生成
	Skydome* skydome_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// DeathParticles* deathParticles_ = nullptr;

	// ゲームの現在のフェーズ(変数)
	Phase phase_;

	// デスフラグ
	bool isDead_ = false;

	// 終了フラグ
	bool finished_ = false;

	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;

	// メンバー変数に追加

	bool isBackgroundStarted_ = false;

	// フェード用
	bool isFadingIn_ = false;
	float fadeScele_ = 0.1f;

	//----------スプライト関係-----------------//
	KamataEngine::Sprite* blackSprite_ = nullptr;
	KamataEngine::Sprite* startSprite_ = nullptr;

	//----------スタートカウントダウンスプライト関係-------------//
	KamataEngine::Sprite* Count1Sprite_ = nullptr;
	KamataEngine::Sprite* Count2Sprite_ = nullptr;
	KamataEngine::Sprite* Count3Sprite_ = nullptr;

	//----------スプライト関係-----------------//

	bool isStarting_ = false;   // スタート演出中かどうか
	bool isGameActive_ = false; // プレイヤー・敵が動ける状態か
	int startTimer_ = 0;        // スタート演出用タイマー

	int currentCount_ = 3;
	float currentScale_ = 1.0f;
	float currentAlpha_ = 1.0f;

	//-----------------------スコア関係--------------------------//
	Score score_;

	int ScoresTexture[10];

	int currentScore_ = score_.GetScore();

	const int kClearScore = 100;

	// 進捗バー用
	uint32_t progressTex_ = 0;
	KamataEngine::Sprite* progressBase_;
	KamataEngine::Sprite* progressFill_;

	// --- エフェクト ---
	float effectAlpha_ = 0.0f;
	float effectScale_ = 1.0f;
	bool isNearGoalEffect_ = false;

	KamataEngine::Sprite* scoreMaxSprite_ = nullptr;
	KamataEngine::Sprite* scoreMaxSprite2_ = nullptr;
	KamataEngine::Sprite* scoreMaxSprite3_ = nullptr;

	//------------------------時間制限------------------------------//
	// 制限時間
	Timer* timer_ = nullptr;
	int numberTextures_[10]; // 制限時間 0～9のテクスチャハンドル

	//------------------------敵のスポーン関係------------------------//
	SpawnManager* spawnManager_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};