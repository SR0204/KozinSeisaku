#include "EnemyDeathParticles.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace KamataEngine;

float RandRange(float min, float max) { return min + (max - min) * (rand() / (float)RAND_MAX); }

EnemyDeathParticles::~EnemyDeathParticles() {
	for (auto& wt : worldTransforms_) {
		delete wt;
	}
	worldTransforms_.clear();
}

void EnemyDeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;

	// パーティクルを kNumParticles 個作る
	worldTransforms_.clear();
	worldTransforms_.resize(kNumParticles); // kNumParticles 個にリサイズ
	for (auto& wt : worldTransforms_) {
		wt = new WorldTransform();
		wt->Initialize();
		wt->translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 0, 1}; // 黄色っぽくマリオ風
	counter_ = 0.0f;
	isFinished_ = false;
}

void EnemyDeathParticles::Update() {
	if (isFinished_)
		return;

	for (auto& wt : worldTransforms_) {
		// ランダム方向に少し飛ばす
		float angle = RandRange(0.0f, 2.0f * 3.14159f);
		float speed = RandRange(0.05f, 0.15f);
		Vector3 velocity = {cos(angle) * speed, sin(angle) * speed, 0.0f};

		wt->translation_ += velocity;

		// 徐々に縮小
		float lifeRatio = counter_ / kDuration;
		wt->scale_ = {initialScale_ * (1.0f - lifeRatio), initialScale_ * (1.0f - lifeRatio), initialScale_ * (1.0f - lifeRatio)};
	}

	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		isFinished_ = true;
		counter_ = kDuration;
	}

	// フェードアウト
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	objectColor_.SetColor(color_);

	// 各ワールド変換更新
	for (auto& wt : worldTransforms_) {
		wt->UpdateMatrix();
	}
}

void EnemyDeathParticles::Draw() {
	if (isFinished_)
		return;

	for (auto& wt : worldTransforms_) {
		model_->Draw(*wt, *camera_, &objectColor_);
	}
}
