#include "TitleScene.h"
#include "SceneID.h"
#include "SceneManager.h"
#include <Audio/Audio.h>
#include <cmath>

using namespace KamataEngine;

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {
	delete sprite_;
	delete sprite2_;
	delete BackGround_;

	// シーン終了時にBGM停止
	if (bgmVoiceHandle_ != -1) {
		Audio::GetInstance()->StopWave(bgmVoiceHandle_);
	}
}

void TitleScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	TitleTextureHandle_ = TextureManager::Load("./Resources/Title/TitleKey.png");
	TitleTextureHandle2_ = TextureManager::Load("./Resources/Title/Title.png");
	TitleBackGroundTextureHandle_ = TextureManager::Load("./Resources/Title/TitleBack.png");

	sprite_ = KamataEngine::Sprite::Create(TitleTextureHandle_, {0, 0});
	sprite2_ = KamataEngine::Sprite::Create(TitleTextureHandle2_, {0, 0});
	BackGround_ = KamataEngine::Sprite::Create(TitleBackGroundTextureHandle_, {0, 0});

	// BGMロード＆再生
	bgmHandle_ = Audio::GetInstance()->LoadWave("./Resources/Sound/TitleBGM.mp3");
	bgmVoiceHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	bgmVolume_ = 1.0f;
	isFadingOut_ = false;
}

void TitleScene::Update() {
	frameCount_++;

	if (isTitle_) {
		if (input_->TriggerKey(DIK_SPACE)) {
			isFadingOut_ = true;
			isTitle_ = false;
			// ゲーム本編に切り替え
			sceneManager_->ChangeScene(SceneID::Game);
			return;
		}

		// タイトルキーの上下移動
		float y = 10 * sin(frameCount_ * 0.05f);
		sprite_->SetPosition({0.0f, y});
	}

	// フェードアウト中の音量調整
	if (isFadingOut_ && bgmVoiceHandle_ != -1) {
		bgmVolume_ -= 0.01f; // 音量を徐々に下げる
		if (bgmVolume_ <= 0.0f) {
			bgmVolume_ = 0.0f;
			Audio::GetInstance()->StopWave(bgmVoiceHandle_);
			bgmVoiceHandle_ = -1;
			// フェードアウトが終わったらシーン切り替え
			sceneManager_->ChangeScene(SceneID::Game);
			return;
		}
		Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);
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

	if (BackGround_) {
		BackGround_->Draw();
	}

	if (isTitle_ && frameCount_ % 150 >= 30) {
		sprite_->Draw();
	}
	if (isTitle_) {
		sprite2_->Draw();
	}
	KamataEngine::Sprite::PostDraw();
}
