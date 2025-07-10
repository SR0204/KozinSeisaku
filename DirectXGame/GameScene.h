#pragma once

#include "CameraManager.h"
#include <audio/Audio.h>
// #include "DeathParticles.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "Player.h"
#include "Skydome.h"
#include <2d/Sprite.h>
#include <3d/DebugCamera.h>
#include <3d/Model.h>
#include <base/DirectXCommon.h>
#include <input/Input.h>
// #include "TitleScene.h"
#include <3d/Camera.h>
#include <3d/WorldTransform.h>
#include <vector>
#include"PhaseManager.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

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
	void Initialize();

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

		kPlay,  // ゲームプレイ
		kDeath, // デス演出
	};

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// デスフラグのgeeter
	bool IsFinished() const { return finished_; }

private: // メンバ変数（関数）
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	//--------------------マネージャー系統----------------------------//
	MapManager* mapManager_;

	EnemyManager* enemyManager_ = nullptr;

	PhaseManager* phaseManager_ = nullptr;

	CameraManager* cameraManager_ = nullptr;

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

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};