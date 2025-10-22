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
	delete BackGround_[0];
	delete BackGround_[1];
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

	// ---------------- ゲームオーバー背景 ----------------
	GameOverBgSprite_ = TextureManager::Load("./Resources/GameOver/GameOverBG.png");
	BackGround_[0] = KamataEngine::Sprite::Create(GameOverBgSprite_, {0, 0});
	BackGround_[1] = KamataEngine::Sprite::Create(GameOverBgSprite_, {1280, 0}); // 右にもう1枚配置

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

	// ---------------- 縦揺れ ----------------
	float bounce = std::sin(frameCount_ * 0.08f) * 6.0f;
	gameOverWT_.translation_.y = 50.0f + bounce;
	gameOverWT_.UpdateMatrix();

	// ---------------- 点滅 ----------------
	blinkAlpha_ = 0.5f + 0.5f * sin(frameCount_ * 0.1f);
	objectColor_.SetColor({1.0f, 0.2f + 0.2f * blinkAlpha_, 0.2f, 1.0f});

	// ---------------- フェードアウトトリガー ----------------
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

	// === 背景をループスクロール ===
	for (int i = 0; i < 2; i++) {
		Vector2 pos = BackGround_[i]->GetPosition();
		pos.x -= bgScrollSpeed_; // 左方向にスクロール
		// 左に完全に出たら右側に回す
		if (pos.x <= -1250) {
			pos.x += 1280 * 2;
		}
		BackGround_[i]->SetPosition(pos);
	}
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// ----------------② 3Dモデル（中央のGameOver文字）----------------
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (gameOverModel_) {
		gameOverModel_->Draw(gameOverWT_, *camera_, &objectColor_);
	}
	Model::PostDraw();

	KamataEngine::Sprite::PreDraw(commandList);

	//--------------------① 背景（最背面）-----------------
	for (int i = 0; i < 2; i++) {
		if (BackGround_[i]) {
			BackGround_[i]->Draw();
		}
	}

	// ----------------③ フェード（最前面）----------------
	if (fadeSprite_) {
		fadeSprite_->Draw();
	}
	KamataEngine::Sprite::PostDraw();
}
