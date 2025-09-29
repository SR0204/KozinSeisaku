#include "GameOverScene.h"
#include "SceneID.h"
#include "SceneManager.h"
#include <cmath>

using namespace KamataEngine;

GameOverScene::GameOverScene() {}
GameOverScene::~GameOverScene() { delete gameOverSprite_; }

void GameOverScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// GameOver用の画像読み込み
	gameOverTextureHandle_ = TextureManager::Load("./Resources/GameOver/GameOver.png");
	gameOverSprite_ = KamataEngine::Sprite::Create(gameOverTextureHandle_, {0, 0});
}

void GameOverScene::Update() {
	if (!gameOverSprite_)return;

	frameCount_++;

	// Enterキーでタイトルに戻る
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene(SceneID::TitleScene);
	}

	// 落下演出（例: 上からY=100まで）
	Vector2 pos = gameOverSprite_->GetPosition();
	const float targetY = 100.0f;
	const float speed = 1.0f;
	if (pos.y < targetY) {
		pos.y += speed;
		if (pos.y > targetY)
			pos.y = targetY;
	}
	gameOverSprite_->SetPosition(pos);
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);
	gameOverSprite_->Draw();
	KamataEngine::Sprite::PostDraw();
}
