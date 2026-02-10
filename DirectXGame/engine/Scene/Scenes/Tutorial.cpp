#include "Tutorial.h"
#include <numbers>

void TutorialUI::Initialize() {
	uint32_t tex = TextureManager::Load("./Resources/OperationHelp/OperationHelp.dds");
	sprite_.reset(Sprite::Create(tex, {640.0f, 360.0f}));

	sprite_->SetAnchorPoint({0.5f, 0.5f});
	sprite_->SetSize({1280.0f, 750.0f});

	alpha_ = 0.0f;
	fadeState_ = FadeState::None;
	isVisible_ = false;
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
}

void TutorialUI::Draw() {
	if (!isVisible_)
		return;
	if (!sprite_)
		return;
	sprite_->SetColor({1.0f, 1.0f, 1.0f, alpha_});
	sprite_->Draw();
}
