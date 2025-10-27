#include "GameOverScene.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include "3d/ObjectColor.h"
#include <cmath>

using namespace KamataEngine;

GameOverScene::GameOverScene() : fadeAlpha_(1.0f), isFadingOut_(false), isFadingIn_(true), frameCount_(0), blinkAlpha_(1.0f), waitAfterFade_(false), waitTimer_(0) {}

GameOverScene::~GameOverScene() {
	delete fadeSprite_;
	delete gameOverModel_;
	delete camera_;

	for (int i = 0; i < 2; i++) {
		delete backgroundModel_[i];
	}
}

void GameOverScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	// ---------------- カメラ ----------------
	camera_ = new Camera();
	camera_->Initialize();
	camera_->translation_ = {0.0f, 50.0f, -200.0f};
	camera_->UpdateMatrix();

	// ---------------- 背景（3Dモデル） ----------------
	for (int i = 0; i < 2; i++) {
		backgroundModel_[i] = Model::CreateFromOBJ("GameOverBG", true);
		backgroundWT_[i].Initialize();
		backgroundWT_[i].translation_ = {1280.0f * (i - 0.5f), 0.0f, 500.0f}; // カメラより奥側
		backgroundWT_[i].scale_ = {100.0f, 100.0f, 1.0f};
		backgroundWT_[i].rotation_ = {0.0f, 0.0f, 0.0f};
		backgroundWT_[i].UpdateMatrix();
	}

	// ---------------- GameOverモデル ----------------
	gameOverModel_ = Model::CreateFromOBJ("GameOver", true);
	gameOverWT_.Initialize();
	gameOverWT_.translation_ = {0.0f, 50.0f, 0.0f};
	gameOverWT_.scale_ = {10.0f, 10.0f, 10.0f};
	gameOverWT_.rotation_.y = -1.5f;
	gameOverWT_.UpdateMatrix();

	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 0.5f, 0.5f, 1.0f});

	// ---------------- 黒フェードスプライト ----------------
	uint32_t blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_ = Sprite::Create(blackTex, {640, 360});
	fadeSprite_->SetAnchorPoint({0.5f, 0.5f});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
}

void GameOverScene::Update() {
	frameCount_++;

	// フェードイン処理
	if (isFadingIn_) {
		fadeAlpha_ -= 0.02f;
		if (fadeAlpha_ <= 0.0f) {
			fadeAlpha_ = 0.0f;
			isFadingIn_ = false;
		}
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
		return;
	}

	// 縦揺れ
	float bounce = std::sin(frameCount_ * 0.08f) * 6.0f;
	gameOverWT_.translation_.y = 50.0f + bounce;
	gameOverWT_.UpdateMatrix();

	// 点滅カラー
	blinkAlpha_ = 0.5f + 0.5f * sin(frameCount_ * 0.1f);
	objectColor_.SetColor({1.0f, 0.2f + 0.2f * blinkAlpha_, 0.2f, 1.0f});

	// フェードアウトトリガー
	if (!isFadingOut_ && !waitAfterFade_ && input_->TriggerKey(DIK_SPACE)) {
		isFadingOut_ = true;
	}

	// フェードアウト処理
	if (isFadingOut_) {
		fadeAlpha_ += 0.02f;
		if (fadeAlpha_ >= 1.0f) {
			fadeAlpha_ = 1.0f;
			isFadingOut_ = false;
			waitAfterFade_ = true;
			waitTimer_ = 0;
		}
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
	}

	// 黒画面待機
	if (waitAfterFade_) {
		waitTimer_++;
		if (waitTimer_ > 120) {
			sceneManager_->RequestScene(SceneID::TitleScene);
		}
	}

	// === 背景モデルのループスクロール ===
	for (int i = 0; i < 2; i++) {
		backgroundWT_[i].translation_.x -= bgScrollSpeed_;
		if (backgroundWT_[i].translation_.x <= -1280.0f) {
			backgroundWT_[i].translation_.x += 1280.0f * 2.0f;
		}
		backgroundWT_[i].UpdateMatrix();
	}
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// ----------------① 背景モデル（最背面）----------------
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	for (int i = 0; i < 2; i++) {
		if (backgroundModel_[i]) {
			backgroundModel_[i]->Draw(backgroundWT_[i], *camera_);
		}
	}
	Model::PostDraw();

	// ----------------② GameOver 3D文字 ----------------
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (gameOverModel_) {
		gameOverModel_->Draw(gameOverWT_, *camera_, &objectColor_);
	}
	Model::PostDraw();

	// ----------------③ フェード（最前面）----------------
	KamataEngine::Sprite::PreDraw(commandList);
	if (fadeSprite_) {
		fadeSprite_->Draw();
	}
	KamataEngine::Sprite::PostDraw();
}
