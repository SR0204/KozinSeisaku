#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include "ClearScene.h"
#include <cmath>

using namespace KamataEngine;

ClearScene::ClearScene()
    : ClearSprite_(nullptr), pressSpaceSprite_(nullptr), fadeSprite_(nullptr), fadeAlpha_(1.0f), isFadingOut_(false), isFadingIn_(true), frameCount_(0), blinkAlpha_(1.0f), waitAfterFade_(false),
      waitTimer_(0) {}

ClearScene::~ClearScene() {
	delete ClearSprite_;
	delete pressSpaceSprite_;
	delete fadeSprite_;
}

void ClearScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// PRESS SPACE（点滅する）
	ClearTextureHandle_ = TextureManager::Load("./Resources/GameOver/GameOver.png");
	ClearSprite_ = KamataEngine::Sprite::Create(ClearTextureHandle_, {640, 360});
	ClearSprite_->SetAnchorPoint({0.5f, 0.5f});

	// Clear画像
	ClearTextureHandle2_ = TextureManager::Load("./Resources/GameClear/Clear.png");
	pressSpaceSprite_ = KamataEngine::Sprite::Create(ClearTextureHandle2_, {640, 520});
	pressSpaceSprite_->SetAnchorPoint({0.5f, 0.5f});

	// 黒フェードスプライト
	uint32_t blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_ = KamataEngine::Sprite::Create(blackTex, {640, 360});
	fadeSprite_->SetAnchorPoint({0.5f, 0.5f});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_}); // 最初は黒く覆う
}

void ClearScene::Update() {
	frameCount_++;

	// ---------------- フェードイン ----------------
	if (isFadingIn_) {
		fadeAlpha_ -= 0.02f;
		if (fadeAlpha_ <= 0.0f) {
			fadeAlpha_ = 0.0f;
			isFadingIn_ = false;
		}
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
		return;
	}

	// ---------------- PRESS SPACE の点滅 ----------------
	blinkAlpha_ = 0.5f + 0.5f * sin(frameCount_ * 0.1f);
	if (pressSpaceSprite_) {
		pressSpaceSprite_->SetColor({1.0f, 1.0f, 1.0f, blinkAlpha_});
	}

	// ---------------- スペース押下でフェードアウト開始 ----------------
	if (!isFadingOut_ && !waitAfterFade_ && input_->TriggerKey(DIK_SPACE)) {
		isFadingOut_ = true;
	}

	// ---------------- フェードアウト（黒くなる） ----------------
	if (isFadingOut_) {
		fadeAlpha_ += 0.02f;
		if (fadeAlpha_ >= 1.0f) {
			fadeAlpha_ = 1.0f;
			isFadingOut_ = false;
			waitAfterFade_ = true; // ←★ フェード完了後に待機開始
			waitTimer_ = 0;
		}
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
	}

	// ---------------- 黒画面のまま待機 ----------------
	if (waitAfterFade_) {
		waitTimer_++;

		// 約2秒（60fps換算で120フレーム）待機
		if (waitTimer_ > 120) {
			sceneManager_->RequestScene(SceneID::TitleScene);
		}
	}
}

void ClearScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	// GameOver本体
	if (ClearSprite_) {
		ClearSprite_->Draw();
	}

	// PRESS SPACE
	if (pressSpaceSprite_) {
		pressSpaceSprite_->Draw();
	}

	// 黒フェード（最後に描画）
	if (fadeSprite_) {
		fadeSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}
