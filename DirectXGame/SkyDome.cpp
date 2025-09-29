#include "SkyDome.h"

void SkyDome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {

	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	camera_ = camera;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
	//// 球の大きさ（スケール）を設定
	// worldTransform_.scale_ = {0.1f, 0.1f, 0.1f}; // 適切な大きさに調整
}

void SkyDome::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
	worldTransform_.rotation_.y += 0.008f; // 回転速度は調整可能
	// 行列計算
	worldTransform_.UpdateMatrix();
}

void SkyDome::Draw() {
	// 3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}