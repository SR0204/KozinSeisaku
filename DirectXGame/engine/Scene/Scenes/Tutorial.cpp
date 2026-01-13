#include "Tutorial.h"
#include <numbers>

void TutorialUI::Initialize(Camera* camera) {
	camera_ = camera;

	model_ = Model::CreateFromOBJ("OperationHelp", true);

	transform_.Initialize();
	transform_.translation_ = {0.0f, 1.0f, -10.0f};
	transform_.scale_ = {3.5f, 3.5f, 3.5f};

	// ★ 正面補正（左向き → 正面）
	transform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;

	alpha_ = 0.0f;
}

void TutorialUI::Show() {
	fadeState_ = FadeState::FadeIn;
	isVisible_ = true;
}

void TutorialUI::Hide() { fadeState_ = FadeState::FadeOut; }

void TutorialUI::Update() {
	if (fadeState_ == FadeState::FadeIn) {
		alpha_ += 0.05f;
		if (alpha_ >= 1.0f) {
			alpha_ = 1.0f;
			fadeState_ = FadeState::None;
		}
	} else if (fadeState_ == FadeState::FadeOut) {
		alpha_ -= 0.05f;
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			fadeState_ = FadeState::None;
			isVisible_ = false;
		}
	}

	transform_.UpdateMatrix();
}

void TutorialUI::Draw() {
	if (!isVisible_)
		return;
	if (!model_)
		return;
	model_->SetAlpha(alpha_);
	model_->Draw(transform_, *camera_);
}
