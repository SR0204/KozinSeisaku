#include "ScorePopUp.h"
#include <2d/Sprite.h>

using namespace KamataEngine;

void ScorePopUp::Initialize(const Vector3& worldPos, int score) {
	position_ = worldPos;
	score_ = score;
}

void ScorePopUp::Update() {
	timer_++;
	position_.y += 0.05f; // 上にふわっと
	alpha_ = 1.0f - (timer_ / lifeTime_);
}

void ScorePopUp::Draw(const Camera& camera) {
	if (alpha_ <= 0.0f)
		return;

	Vector2 screenPos = camera.WorldToScreen(position_);

	// Sprite描画例（フォントを使うなら DebugText に置き換えてOK）
	Sprite::Draw("+" + std::to_string(score_), screenPos.x, screenPos.y, {1, 1, 1, alpha_});
}