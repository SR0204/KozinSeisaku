#pragma once
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <audio/Audio.h>
#include <base/DirectXCommon.h>
#include <input/Input.h>

/// <summary>
/// 天球
/// </summary>
class Skydome {

public:
	// メンバ関数

	/// <summary>
	/// コンストクラタ
	/// </summary>
	Skydome();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Skydome();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデルデータ
	KamataEngine::Model* model_ = nullptr;

	// ビュープロジェクション
	KamataEngine::Camera* camera_ = nullptr;
};