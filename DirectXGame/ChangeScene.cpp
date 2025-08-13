// ChangeScene.cpp
#include "ChangeScene.h"
#include <algorithm>
#include <iostream>

void ChangeScene::Start(std::function<void()> changeFunc, float fadeTime) {
	if (state_ != ChangeState::None)
		return; // 切り替え中なら無視
	changeFunc_ = changeFunc;
	fadeTime_ = fadeTime;
	timer_ = 0.0f;
	state_ = ChangeState::FadeOut;
}

void ChangeScene::Update(float deltaTime) {
	if (state_ == ChangeState::None)
		return;

	timer_ += deltaTime;

	switch (state_) {
	case ChangeState::FadeOut:
		if (timer_ >= fadeTime_) {
			if (changeFunc_)
				changeFunc_(); // シーン切り替え
			timer_ = 0.0f;
			state_ = ChangeState::FadeIn;
		}
		break;

	case ChangeState::FadeIn:
		if (timer_ >= fadeTime_) {
			timer_ = 0.0f;
			state_ = ChangeState::None;
		}
		break;

	default:
		break;
	}
}

void ChangeScene::Draw() {
	if (state_ == ChangeState::None)
		return;

	float alpha = 0.0f;
	if (state_ == ChangeState::FadeOut) {
		alpha = timer_ / fadeTime_;
	} else if (state_ == ChangeState::FadeIn) {
		alpha = 1.0f - (timer_ / fadeTime_);
	}
	alpha = std::clamp(alpha, 0.0f, 1.0f);

	// コンソール出力でフェード確認
	std::cout << "[フェード中 α=" << alpha << "]\n";
}
