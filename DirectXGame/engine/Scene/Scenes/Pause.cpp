#include "Pause.h"

using namespace KamataEngine;

void Pause::Initialize(SceneManager* sceneManager) {
	isPoused_ = false;
	input_ = Input::GetInstance();
	sceneManager_ = sceneManager;

	// ポーズPNG読み込み
	uint32_t tex = TextureManager::Load("./Resources/Pause/Pause.png");
	PauseSprite_ = Sprite::Create(tex, {640.0f, 360.0f});
	PauseSprite_->SetAnchorPoint({0.5f, 0.5f});
	PauseSprite_->SetSize({1280.0f, 720.0f});
	fadeState_ = PauseFadeState::None;

	// ポーズ画面~タイトル用黒フェード
	uint32_t blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_ = Sprite::Create(blackTex, {640, 360});
	fadeSprite_->SetAnchorPoint({0.5f, 0.5f});
	fadeSprite_->SetSize({1280, 720});

	alpha_ = 0.0f;
}

void Pause::Update() {

	// ESCでポーズ切り替え
	if (input_->TriggerKey(DIK_ESCAPE)) {
		if (!isPoused_) {
			// ポーズ開始 → フェードイン
			isPoused_ = true;
			fadeState_ = PauseFadeState::FadeIn;
		} else {
			// ポーズ解除 → フェードアウト
			fadeState_ = PauseFadeState::FadeOut;
		}
	}

	// Tでタイトル移動
	if (isPoused_ && input_->TriggerKey(DIK_T)) {
		sceneFadeState_ = SceneFadeState::FadeOut;
		fadeAlpha_ = 0.0f;
	}

	// ===== フェード処理 =====
	const float fadeSpeed = 0.05f; // フレーム依存（60fpsで約0.3秒）

	if (fadeState_ == PauseFadeState::FadeIn) {
		alpha_ += fadeSpeed;
		if (alpha_ >= 1.0f) {
			alpha_ = 1.0f;
			fadeState_ = PauseFadeState::None;
		}
	} else if (fadeState_ == PauseFadeState::FadeOut) {
		alpha_ -= fadeSpeed;
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			fadeState_ = PauseFadeState::None;
			isPoused_ = false; // ★ここで解除
		}
	}

	// ===== ポーズ画面~タイトル用黒フェード処理 =====
	if (sceneFadeState_ == SceneFadeState::FadeOut) {
		fadeAlpha_ += 0.03f;

		if (fadeAlpha_ >= 1.0f) {
			fadeAlpha_ = 1.0f;

			requestReturnTitle_ = true;
		}
	}
}

void Pause::Draw() {
	if (!isPoused_)
		return;

	PauseSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha_});
	PauseSprite_->Draw();

	// ポーズ画面~タイトル用黒フェード
	if (fadeAlpha_ > 0.0f) {
		fadeSprite_->SetColor({0, 0, 0, fadeAlpha_});
		fadeSprite_->Draw();
	}
}
