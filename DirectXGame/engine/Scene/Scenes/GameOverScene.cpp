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
	delete pressSpaceModel_;
}

void GameOverScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	camera_ = new Camera();
	camera_->Initialize();

	// -----------------------------
	// GameOver 3Dモデル読み込み
	// -----------------------------
	gameOverModel_ = Model::CreateFromOBJ("GameOver", true);
	gameOverWT_.Initialize();
	gameOverWT_.translation_ = {0.0f, 50.0f, 0.0f};
	gameOverWT_.scale_ = {1.0f, 1.0f, 1.0f};

	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 0.5f, 0.5f, 1.0f});

	// -----------------------------
	// PRESS SPACE 3Dモデル
	// -----------------------------
	//pressSpaceModel_ = Model::CreateFromOBJ("GameOverPushKey", true);
	pressSpaceWT_.Initialize();
	pressSpaceWT_.translation_ = {0.0f, -30.0f, 0.0f};
	pressSpaceWT_.scale_ = {0.5f, 0.5f, 0.5f};

	pressSpaceColor_.Initialize();
	pressSpaceColor_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// -----------------------------
	// 黒フェードスプライト
	// -----------------------------
	uint32_t blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_ = Sprite::Create(blackTex, {640, 360});
	fadeSprite_->SetAnchorPoint({0.5f, 0.5f});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
}

void GameOverScene::Update() {
	frameCount_++;

	camera_->translation_ = {0.0f, 50.0f, -200.0f};
	camera_->UpdateMatrix();

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

	// ---------------- PRESS SPACE 点滅 ----------------
	blinkAlpha_ = 0.5f + 0.5f * sin(frameCount_ * 0.1f);
	pressSpaceColor_.SetColor({1.0f, 1.0f, 1.0f, blinkAlpha_});

	// ---------------- スペース押下でフェードアウト開始 ----------------
	if (!isFadingOut_ && !waitAfterFade_ && input_->TriggerKey(DIK_SPACE)) {
		isFadingOut_ = true;
	}

	// ---------------- フェードアウト ----------------
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

	// ---------------- 黒画面待機 ----------------
	if (waitAfterFade_) {
		waitTimer_++;
		if (waitTimer_ > 120) {
			sceneManager_->RequestScene(SceneID::TitleScene);
		}
	}

	// ---------------- 3Dモデル揺れ・発光 ----------------
	float bounce = std::sin(frameCount_ * 0.08f) * 6.0f;
	gameOverWT_.translation_.y = 50.0f + bounce;

	float glow = 0.5f + 0.5f * std::sin(frameCount_ * 0.05f);
	objectColor_.SetColor({1.0f, 0.2f + glow * 0.2f, 0.2f, 1.0f});

	// PRESS SPACE も少し上下揺れ
	float pressBounce = std::sin(frameCount_ * 0.1f) * 3.0f;
	pressSpaceWT_.translation_.y = -30.0f + pressBounce;
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// -----------------------------
	// 黒フェードスプライト
	// -----------------------------
	KamataEngine::Sprite::PreDraw(commandList);
	if (fadeSprite_)
		fadeSprite_->Draw();
	KamataEngine::Sprite::PostDraw();

	// -----------------------------
	// GameOver 3Dモデル描画
	// -----------------------------
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (gameOverModel_)
		gameOverModel_->Draw(gameOverWT_, *camera_, &objectColor_);
	/*if (pressSpaceModel_)
		pressSpaceModel_->Draw(pressSpaceWT_, *camera_, &pressSpaceColor_);*/
	Model::PostDraw();
}
