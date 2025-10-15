#include "TitleScene.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include <Audio/Audio.h>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

TitleScene::TitleScene() : bgmHandle_(-1), bgmVoiceHandle_(-1), bgmVolume_(1.0f), isFadingOut_(false), frameCount_(0), isTitle_(true), fadeAlpha_(0.0f), bounceTimer_(0) {}

TitleScene::~TitleScene() {
	delete sprite_;
	delete sprite2_;
	delete BackGround_[1];

	if (bgmVoiceHandle_ != -1) {
		Audio::GetInstance()->StopWave(bgmVoiceHandle_);
	}
}

void TitleScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// === 2Dリソース ===
	TitleTextureHandle_ = TextureManager::Load("./Resources/Title/TitleKey.png");
	TitleBackGroundTextureHandle_ = TextureManager::Load("./Resources/Title/TitleBack.png");
	fadeTextureHandle_ = TextureManager::Load("./Resources/Title/fadeTexture.png");

	sprite_ = KamataEngine::Sprite::Create(TitleTextureHandle_, {0, 0});

	// === 背景スプライトを2枚作る ===
	BackGround_[0] = KamataEngine::Sprite::Create(TitleBackGroundTextureHandle_, {0, 0});
	BackGround_[1] = KamataEngine::Sprite::Create(TitleBackGroundTextureHandle_, {1280, 0}); // 右にもう1枚配置

	fadeSprite_ = KamataEngine::Sprite::Create(fadeTextureHandle_, {0, 0});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, 0.0f});

	// === 3Dモデル ===
	titleModel_ = KamataEngine::Model::CreateFromOBJ("Title", true);
	titleTransform_.Initialize();

	// 正面向きに調整
	titleTransform_.rotation_.y = -1.5f;

	titleTransform_.scale_ = {3.0f, 3.0f, 3.0f};
	titleTransform_.translation_ = {0.0f, 10.0f, 0.0f};
	titleTransform_.UpdateMatrix();

	Camera_.Initialize();

	// === 音 ===
	bgmHandle_ = Audio::GetInstance()->LoadWave("./Resources/Sound/TitleBGM.mp3");
	bgmVoiceHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	bgmVolume_ = 0.5f;
	Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);

	isFadingOut_ = false;
	fadeAlpha_ = 0.0f;

	// === バウンド設定 ===
	bounceAmplitude_ = 15.0f;
	bounceTimer_ = 0;
	isBounceFinished_ = false;

	// ===  ライト設定 ===
	lightGroup_.reset(KamataEngine::LightGroup::Create());
	lightGroup_->SetDirLightDir(0, {0.3f, -1.0f, 0.4f});
	lightGroup_->SetDirLightColor(0, {1.4f, 1.3f, 1.2f}); // 少し暖色寄り
	lightGroup_->SetAmbientColor({0.9f, 0.8f, 0.7f});     // 明るい雰囲気

	titleModel_->SetLightGroup(lightGroup_.get());
}

void TitleScene::Update() {
	frameCount_++;

	if (isTitle_ && input_->TriggerKey(DIK_SPACE)) {
		isTitle_ = false;
		isFadingOut_ = true;
	}

	// === 点滅 ===
	if (isTitle_) {
		blinkTimer_++;
		// 0.5〜1.0の範囲でふんわり明るさ変化（消えない）
		float alpha = (std::sin(blinkTimer_ * 0.05f) * 0.25f + 0.75f);
		// 色も少し明るく（RGB1.2倍）してポップさUP
		sprite_->SetColor({1.2f, 1.2f, 1.2f, alpha});
	}

	// === バウンド演出 ===
	const float targetY = 0.0f;
	const float fallSpeed = 0.5f;

	if (!isBounceFinished_) {
		if (titleTransform_.translation_.y > targetY && bounceTimer_ == 0) {
			titleTransform_.translation_.y -= fallSpeed;
			if (titleTransform_.translation_.y <= targetY) {
				titleTransform_.translation_.y = targetY;
				bounceTimer_ = 60;
				bounceAmplitude_ = 1.0f;
			}
		} else if (bounceTimer_ > 0) {
			float t = (60 - bounceTimer_) / 10.0f;
			titleTransform_.translation_.y = targetY + std::abs(std::sin(t)) * bounceAmplitude_;
			bounceAmplitude_ *= bounceDecay_;
			bounceTimer_--;
			if (bounceTimer_ <= 0 || bounceAmplitude_ < 0.01f) {
				isBounceFinished_ = true;
				titleTransform_.translation_.y = targetY;
			}
		}
	} else {
		// ✨ ゆらゆらアニメ
		titleTransform_.rotation_.y = -1.5f + std::sin(frameCount_ * 0.01f) * 0.2f;
		titleTransform_.rotation_.x = std::sin(frameCount_ * 0.015f) * 0.1f;
		titleTransform_.translation_.y = targetY + std::sin(frameCount_ * 0.03f) * 0.3f;
	}

	titleTransform_.UpdateMatrix();

	// === フェード処理 ===
	if (isFadingOut_) {
		bgmVolume_ -= 0.1f;
		if (bgmVolume_ < 0.0f)
			bgmVolume_ = 0.0f;
		Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);

		fadeAlpha_ += 0.02f;
		if (fadeAlpha_ > 1.0f)
			fadeAlpha_ = 1.0f;
		fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, fadeAlpha_});

		if (fadeAlpha_ >= 1.0f) {
			Audio::GetInstance()->StopWave(bgmVoiceHandle_);
			sceneManager_->ChangeScene(SceneID::Game);
		}
	}

	// === ライト回転（ポップに動かす） ===
	Vector3 lightDir;
	lightDir.x = std::cos(frameCount_ * 0.01f) * 0.6f;
	lightDir.y = -1.0f;
	lightDir.z = std::sin(frameCount_ * 0.01f) * 0.6f;
	lightGroup_->SetDirLightDir(0, lightDir);
	lightGroup_->Update();

	// === 背景をループスクロール ===
	for (int i = 0; i < 2; i++) {
		Vector2 pos = BackGround_[i]->GetPosition();
		pos.x -= bgScrollSpeed_; // 左方向にスクロール
		// 左に完全に出たら右側に回す
		if (pos.x <= -1280) {
			pos.x += 1280 * 2;
		}
		BackGround_[i]->SetPosition(pos);
	}
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// === 3D ===
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (titleModel_) {
		titleModel_->Draw(titleTransform_, Camera_);
	}
	Model::PostDraw();

	// === 2D ===
	KamataEngine::Sprite::PreDraw(commandList);

	// 背景を2枚描画
	for (int i = 0; i < 2; i++) {
		if (BackGround_[i])
			BackGround_[i]->Draw();
	}

	if (isTitle_ && frameCount_ % 150 >= 30) {
		sprite_->Draw();
	}

	if (fadeSprite_)
		fadeSprite_->Draw();

	KamataEngine::Sprite::PostDraw();
}
