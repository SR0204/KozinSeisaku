#include "Skydome.h"
#include <cassert>

using namespace KamataEngine;

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

	// 引数の内容をメンバ変数に記録
	camera_ = camera;
}

void Skydome::Update() {}

void Skydome::Draw() {

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);

}