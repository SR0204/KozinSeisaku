#include "TitleScene.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include <Audio/Audio.h>
#include <cmath>

using namespace KamataEngine;

TitleScene::TitleScene() : bgmHandle_(-1), bgmVoiceHandle_(-1), bgmVolume_(1.0f), isFadingOut_(false), frameCount_(0), isTitle_(true), fadeAlpha_(0.0f), bounceTimer_(0) {}

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

	// テクスチャ読み込み
	TitleTextureHandle_ = TextureManager::Load("./Resources/Title/TitleKey.png");
	TitleTextureHandle2_ = TextureManager::Load("./Resources/Title/Title.png");
	TitleBackGroundTextureHandle_ = TextureManager::Load("./Resources/Title/TitleBack.png");

	// フェード用の黒画像をロード（1x1ピクセルでもOK）
	fadeTextureHandle_ = TextureManager::Load("./Resources/Title/fadeTexture.png");

	sprite_ = KamataEngine::Sprite::Create(TitleTextureHandle_, {0, 0});
	sprite2_ = KamataEngine::Sprite::Create(TitleTextureHandle2_, {0, -100}); // 上から落下
	BackGround_ = KamataEngine::Sprite::Create(TitleBackGroundTextureHandle_, {0, 0});

	// フェードスプライトは画面全体を覆う
	fadeSprite_ = KamataEngine::Sprite::Create(fadeTextureHandle_, {0, 0});
	fadeSprite_->SetSize({1280, 720});               // 画面サイズに拡大
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, 0.0f}); // 最初は透明

	// BGMロード＆再生
	bgmHandle_ = Audio::GetInstance()->LoadWave("./Resources/Sound/TitleBGM.mp3");
	bgmVoiceHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	bgmVolume_ = 1.0f;
	isFadingOut_ = false;
	fadeAlpha_ = 0.0f;
}

void TitleScene::Update() {
	frameCount_++;

	// スペースキーでフェードアウト開始
	if (isTitle_ && input_->TriggerKey(DIK_SPACE)) {
		isTitle_ = false;
		isFadingOut_ = true;
	}

	// PRESS SPACE の点滅演出
	if (isTitle_) {
		float alpha = (sin(frameCount_ * 0.05f) * 0.5f + 0.5f);
		sprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	}

	// ロゴの落下＆バウンド
	Vector2 position = sprite2_->GetPosition();
	const float targetY = 100.0f;
	const float speed = 1.0f;

	if (position.y < targetY) {
		position.y += speed;
		if (position.y > targetY) {
			position.y = targetY;
			bounceTimer_ = 30;
		}
	} else if (bounceTimer_ > 0) {
		position.y += sin(bounceTimer_ * 0.3f) * 2.0f;
		bounceTimer_--;
	}
	sprite2_->SetPosition(position);

	// フェードアウト処理
	if (isFadingOut_) {
		// 音量下げる
		bgmVolume_ -= 0.01f;
		if (bgmVolume_ < 0.0f)
			bgmVolume_ = 0.0f;
		Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);

		// 画面暗転のアルファ値更新
		fadeAlpha_ += 0.02f;
		if (fadeAlpha_ > 1.0f)
			fadeAlpha_ = 1.0f;

		fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, fadeAlpha_});

		// フェード完了後にシーン遷移
		if (fadeAlpha_ >= 1.0f) {
			Audio::GetInstance()->StopWave(bgmVoiceHandle_);
			sceneManager_->ChangeScene(SceneID::Game);
		}
	}
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	// 背景
	if (BackGround_) {
		BackGround_->Draw();
	}

	// タイトルキーとロゴ
	if (isTitle_) {
		if (frameCount_ % 150 >= 30) {
			sprite_->Draw();
		}
		sprite2_->Draw();
	}

	// フェードアウト時の黒い幕
	if (fadeSprite_) {
		fadeSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}
