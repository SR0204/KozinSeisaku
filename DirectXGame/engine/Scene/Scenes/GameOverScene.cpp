#include "GameOverScene.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include <cmath>

using namespace KamataEngine;

GameOverScene::GameOverScene()
    :pressSpaceSprite_(nullptr), fadeSprite_(nullptr), fadeAlpha_(1.0f), isFadingOut_(false), isFadingIn_(true), frameCount_(0), blinkAlpha_(1.0f), waitAfterFade_(false),
      waitTimer_(0) {}

GameOverScene::~GameOverScene() {
	delete gameOverShadow_;
	delete gameOverEdge_;
	delete gameOverMain_;
	delete pressSpaceSprite_;
	delete fadeSprite_;
}

void GameOverScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// GameOver画像
	gameOverTextureHandle_ = TextureManager::Load("./Resources/GameOver/GameOverSprite.png");

	// GameOver(縁取り、影 等)
	gameOverShadow_ = KamataEngine::Sprite::Create(gameOverTextureHandle_, {640, 360});
	gameOverEdge_ = KamataEngine::Sprite::Create(gameOverTextureHandle_, {640, 360});
	gameOverMain_ = KamataEngine::Sprite::Create(gameOverTextureHandle_, {640, 360});

	gameOverShadow_->SetAnchorPoint({0.5f, 0.5f});
	gameOverEdge_->SetAnchorPoint({0.5f, 0.5f});
	gameOverMain_->SetAnchorPoint({0.5f, 0.5f});

	// PRESS SPACE（点滅する）
	gameOverTextureHandle2_ = TextureManager::Load("./Resources/GameOver/GameOver.png");
	pressSpaceSprite_ = KamataEngine::Sprite::Create(gameOverTextureHandle2_, {640, 520});
	pressSpaceSprite_->SetAnchorPoint({0.5f, 0.5f});

	// GameOverBackGround画像
	gameOverBG_ = TextureManager::Load("./Resources/GameOver/GameOverBG.png");
	GameOverBgSprite_ = KamataEngine::Sprite::Create(gameOverBG_, {0, 0});

	// 黒フェードスプライト
	uint32_t blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_ = KamataEngine::Sprite::Create(blackTex, {640, 360});
	fadeSprite_->SetAnchorPoint({0.5f, 0.5f});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_}); // 最初は黒く覆う
}

void GameOverScene::Update() {
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

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	// GameOver背景
	if (GameOverBgSprite_) {
		GameOverBgSprite_->Draw();
	}

	// =====================
	// === GAME OVERロゴ ===
	// =====================
	float bounce = std::sin(frameCount_ * 0.08f) * 6.0f; // 揺れ
	Vector2 basePos = {640.0f, 320.0f + bounce};         // 画面中央より少し上

	// --- ①影（少し下＆右にずらす）---
	gameOverShadow_->SetPosition({basePos.x + 6.0f, basePos.y + 6.0f});
	gameOverShadow_->SetSize({640.0f, 360.0f});
	gameOverShadow_->SetColor({0.0f, 0.0f, 0.0f, 0.7f});
	gameOverShadow_->Draw();

	// --- ②縁取り（少し大きく）---
	gameOverEdge_->SetPosition(basePos);
	gameOverEdge_->SetSize({650.0f, 365.0f});           // 本体よりやや大きめ
	gameOverEdge_->SetColor({1.0f, 0.85f, 0.2f, 1.0f}); // ゴールド
	gameOverEdge_->Draw();

	// --- ③本体（発光赤）---
	float glow = 0.5f + 0.5f * std::sin(frameCount_ * 0.05f);
	gameOverMain_->SetPosition(basePos);
	gameOverMain_->SetSize({640.0f, 360.0f});
	gameOverMain_->SetColor({1.0f, 0.2f + glow * 0.2f, 0.2f, 1.0f});
	gameOverMain_->Draw();

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
