#include "Skydome.h"
#include <cassert>

// インストラクタ
Skydome::Skydome() {}

// デストラクタ
Skydome::~Skydome() {}

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	// NULLポインタチェック
	assert(model);

	// 引数
	model_ = model;

	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Skydome::Update() {}

void Skydome::Draw() {

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
}