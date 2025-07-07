#pragma once

#include "CameraController.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include "Player.h"
#include "Skydome.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/DebugCamera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <audio/Audio.h>
#include <base/DirectXCommon.h>

#include <vector>

class GameScene {

public: // メンバ関数
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

	void GenerateBlocks();

private: // メンバ変数
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	/// ゲームシーン用
	uint32_t textureHandle_ = 0;

	// 3Dモデルの生成
	KamataEngine::Model* model_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	// 3Dモデルデータ
	KamataEngine::Model* modelBlock_ = nullptr;

	// 天球
	Skydome* skyDome_ = nullptr;

	// 天球モデルデータ
	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* modelPlayer_ = nullptr;

	// ビュープロジェクション
	KamataEngine::Camera camera_;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipFiled_;

	// カメラビュープロジェクション
	KamataEngine::Camera* CameraViewProjection_;

	// カメラコントローラー
	CameraController* CameraController_;
};
