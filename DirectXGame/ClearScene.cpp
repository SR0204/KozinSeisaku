#include "ClearScene.h"
#include "SceneID.h"
#include "SceneManager.h"
#include <cmath>

using namespace KamataEngine;

ClearScene::ClearScene() {}
ClearScene::~ClearScene() { delete clearSprite_; }

void ClearScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	clearTextureHandle_ = TextureManager::Load("./Resources/GameClear/Clear.png");
	clearSprite_ = KamataEngine::Sprite::Create(clearTextureHandle_, {0, 0});
}

void ClearScene::Update() {
	frameCount_++;

	// クリアシーンからタイトルに戻る
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene(SceneID::TitleScene);
	}
}

void ClearScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);
	clearSprite_->Draw();
	KamataEngine::Sprite::PostDraw();
}
