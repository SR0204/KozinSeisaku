#include "GameOverScene.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include <cmath> // sin を使うため

using namespace KamataEngine;

GameOverScene::GameOverScene()
    : gameOverSprite_(nullptr), pressSpaceSprite_(nullptr), fadeSprite_(nullptr), fadeAlpha_(1.0f), isFadingOut_(false), isFadingIn_(true), frameCount_(0), blinkAlpha_(1.0f) {}

GameOverScene::~GameOverScene() {
	delete gameOverSprite_;
	delete pressSpaceSprite_;
	delete fadeSprite_;
}

void GameOverScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// GameOver画像（点滅しないメイン部分）
	gameOverTextureHandle_ = TextureManager::Load("./Resources/GameOver/GameOver.png");
	gameOverSprite_ = KamataEngine::Sprite::Create(gameOverTextureHandle_, {0, 0});

	// PRESS SPACE（点滅する部分）
	gameOverTextureHandle2_ = TextureManager::Load("./Resources/GameOver/GameOverSprite.png");
	pressSpaceSprite_ = KamataEngine::Sprite::Create(gameOverTextureHandle2_, {0, 0}); // 位置は調整

	// 黒幕
	int blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_ = KamataEngine::Sprite::Create(blackTex, {0, 0});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
}

void GameOverScene::Update() {
	frameCount_++;

	// フェードイン
	if (isFadingIn_) {
		fadeAlpha_ -= 0.02f;
		if (fadeAlpha_ <= 0.0f) {
			fadeAlpha_ = 0.0f;
			isFadingIn_ = false;
		}
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
		return;
	}

	// PRESS SPACE の文字をなめらかに点滅
	blinkAlpha_ = 0.5f + 0.5f * sin(frameCount_ * 0.1f);
	if (pressSpaceSprite_) {
		pressSpaceSprite_->SetColor({1.0f, 1.0f, 1.0f, blinkAlpha_});
	}

	// スペースキーでフェードアウト
	if (!isFadingOut_ && input_->TriggerKey(DIK_SPACE)) {
		isFadingOut_ = true;
	}

	// フェードアウト
	if (isFadingOut_) {
		fadeAlpha_ += 0.02f;
		if (fadeAlpha_ >= 1.0f) {
			fadeAlpha_ = 1.0f;
			sceneManager_->RequestScene(SceneID::TitleScene);
		}
		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, fadeAlpha_});
	}
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	// 黒い幕
	if (fadeSprite_) {
		fadeSprite_->Draw();
	}

	// 点滅しないGameOver本体
	if (gameOverSprite_) {
		gameOverSprite_->Draw();
	}

	// 点滅するPRESS SPACE
	if (pressSpaceSprite_) {
		pressSpaceSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}
