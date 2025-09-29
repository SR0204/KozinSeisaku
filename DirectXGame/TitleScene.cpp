#include "TitleScene.h"
#include "SceneID.h"
#include "SceneManager.h"
#include <cmath>

using namespace KamataEngine;

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {
	delete sprite_;
	delete sprite2_;
}

void TitleScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	TitleTextureHandle_ = TextureManager::Load("./Resources/Title/TitleKey.png");
	TitleTextureHandle2_ = TextureManager::Load("./Resources/Title/Title.png");

	sprite_ = KamataEngine::Sprite::Create(TitleTextureHandle_, {0, 0});
	sprite2_ = KamataEngine::Sprite::Create(TitleTextureHandle2_, {0, 0});
}

void TitleScene::Update() {
	frameCount_++;

	if (isTitle_) {
		if (input_->TriggerKey(DIK_RETURN)) {
			isTitle_ = false;
			// ゲーム本編に切り替え
			sceneManager_->ChangeScene(SceneID::Game);
			return;
		}

		// タイトルキーの上下移動
		float y = 10 * sin(frameCount_ * 0.05f);
		sprite_->SetPosition({0.0f, y});
	}

	// タイトルロゴの落下演出
	Vector2 position = sprite2_->GetPosition();
	const float targetY = 100.0f;
	const float speed = 1.0f;

	if (position.y < targetY) {
		position.y += speed;
		if (position.y > targetY) {
			position.y = targetY;
		}
	}
	sprite2_->SetPosition(position);
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);
	if (isTitle_ && frameCount_ % 150 >= 30) {
		sprite_->Draw();
	}
	if (isTitle_) {
		sprite2_->Draw();
	}
	KamataEngine::Sprite::PostDraw();
}
