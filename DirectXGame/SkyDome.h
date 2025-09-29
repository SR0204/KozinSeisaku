#pragma once
#include "KamataEngine.h"
#include <assert.h>
#include <numbers>

class SkyDome {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Update();
	void Draw();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_ = nullptr;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// モデルの移動速度や位置を格納する変数
	KamataEngine::Vector3 velocity_; // 移動速度
};