#pragma once

#include "CameraController.h"
#include <audio/Audio.h>
// #include "DeathParticles.h"
#include "Enemy.h"
#include "MapChipField.h"
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

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数(引数）
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	// 敵
	// Enemy* enemy_ = nullptr;

	Model* EnemyModel_ = nullptr;

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

	void GenerateBlocks();

	// 全ての当たり判定を行う
	void CheckAllCollisions();

	// ゲームのフェーズ(型)
	enum class Phase {

		kPlay,  // ゲームプレイ
		kDeath, // デス演出
	};

	// フェーズ切り替え
	void ChangePhase();

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// デスフラグのgeeter
	bool IsFinished() const { return finished_; }

private: // メンバ変数（関数）
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// ビュープロジェクション生成
	Camera camera_;

	// 3Dモデルの生成
	Model* model_ = nullptr;

	// プレイヤーの生成
	Player* player_ = nullptr;

	// 天球の生成
	Skydome* skydome_ = nullptr;

	// 3Dモデル
	Model* modelSkydome_ = nullptr;

	// プレイヤーモデル
	Model* modelPlayer_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// ブロックのモデルを読み込む
	Model* modelBlock_ = 0;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// 敵の複数化
	std::list<Enemy*> enemies_;

	// 敵
	Model* modelEnemy_;

	// デバッグカメラの有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラの生成
	DebugCamera* debugCamera_ = nullptr;

	// カメラコントローラー
	CameraController* cameraController_;

	// DeathParticles* deathParticles_ = nullptr;

	Model* deathParticleModel_ = nullptr;

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