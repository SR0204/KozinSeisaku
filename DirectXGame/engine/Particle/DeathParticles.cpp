#include "DeathParticles.h"
#include <algorithm>
#include <cassert>

using namespace KamataEngine;

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// Nullポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_= camera;

	// ワールド変換の初期化
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();

	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {

	// ワールド変換の更新
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};

		// 回転角を計算する
		float angle = kAngleUnit * i;
		// Z軸回り回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);

		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);

		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	// 行列の更新
	for (auto& worldTransform : worldTransforms_) {

		worldTransform.UpdateMatrix();
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {

		counter_ = kDuration;

		// 終了扱いにする
		isFinished_ = true;
	}

	// 終了なら何もしない
	if (isFinished_) {

		return;
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);

	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	// 色変更オブジェクトをVRAMに転送
	//objectColor_.GetColor();
}

void DeathParticles::Draw() {

	// 終了なら何もしない
	if (isFinished_) {

		return;
	}

	// 3Dモデルを描画
	for (auto& worldTransform : worldTransforms_) {

		// model_->Draw(worldTransform, *viewProjection_);
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}