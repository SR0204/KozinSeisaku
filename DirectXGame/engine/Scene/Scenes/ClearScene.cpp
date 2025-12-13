#define NOMINMAX
#include "ClearScene.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"
#include <Audio/Audio.h>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <string> // std::string
#include <vector> // std::vector

using namespace KamataEngine;

ClearScene::ClearScene() : bgmHandle_(-1), bgmVoiceHandle_(-1), bgmVolume_(1.0f), isFadingOut_(false), frameCount_(0), isClear_(true), fadeAlpha_(0.0f), bounceTimer_(0) {}

ClearScene::~ClearScene() {
	delete sprite_;
	delete sprite2_;
	delete BackGround_[1];

	if (bgmVoiceHandle_ != -1) {
		Audio::GetInstance()->StopWave(bgmVoiceHandle_);
	}
}

void ClearScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// === 2Dリソース ===
	ClearTextureHandle_ = TextureManager::Load("./Resources/Title/TitleKey.png");
	ClearBackGroundTextureHandle_ = TextureManager::Load("./Resources/ClearBG/ClearBG.png");
	fadeTextureHandle_ = TextureManager::Load("./Resources/Title/fadeTexture.png");

	sprite_ = KamataEngine::Sprite::Create(ClearTextureHandle_, {0, 0});

	// === 背景スプライトを2枚作る ===

	BackGround_[0] = KamataEngine::Sprite::Create(ClearBackGroundTextureHandle_, {0, 0});
	bgSize_ = BackGround_[0]->GetSize();
	BackGround_[1] = KamataEngine::Sprite::Create(ClearBackGroundTextureHandle_, {bgSize_.x, 0}); // 右にもう1枚配置

	fadeSprite_ = KamataEngine::Sprite::Create(fadeTextureHandle_, {0, 0});
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, 0.0f});

	// === 3Dモデル ===
	ClearModel_ = KamataEngine::Model::CreateFromOBJ("Clear", true);
	ClearTransform_.Initialize();

	baseRotY = 5.0f;

	// 正面向きに調整
	ClearTransform_.rotation_ = {0.0f, baseRotY, 0.0f};

	ClearTransform_.scale_ = {8.5f, 8.5f, 8.5f};
	ClearTransform_.translation_ = {-26.0f, 15.0f, 0.0f};
	ClearTransform_.UpdateMatrix();

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
	lightGroup_->SetDirLightColor(0, {0.6f, 0.2f, 0.2f});
	lightGroup_->SetAmbientColor({0.2f, 0.05f, 0.05f});

	ClearModel_->SetLightGroup(lightGroup_.get());

	//=================================スコア関係=====================

	// SceneManager から最終スコアを取得
	finalScore_ = sceneManager_->GetFinalScore();

	displayScore_ = 0; // 最初は０からスタート

	// === 数字スプライト読み込み ===
	for (int i = 0; i < 10; i++) {
		std::string path = "./Resources/Numbers/number_" + std::to_string(i) + ".png";
		uint32_t tex = TextureManager::Load(path);
		numberSprites_[i] = KamataEngine::Sprite::Create(tex, {0, 0});
	}

	//=============光のエフェクト===============
	uint32_t shineTex = TextureManager::Load("./Resources/Effect/shine.png");
	shineSprite_ = KamataEngine::Sprite::Create(shineTex, {0, 0});
	shineSprite_->SetSize({200, 40});
	shineSprite_->SetColor({1, 1, 1, 0}); // 初期は透明
}

void ClearScene::Update() {
	frameCount_++;

	if (isClear_ && input_->TriggerKey(DIK_SPACE)) {
		isClear_ = false;
		isFadingOut_ = true;
	}

	// === 点滅 ===
	if (isClear_) {
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
		if (ClearTransform_.translation_.y > targetY && bounceTimer_ == 0) {
			ClearTransform_.translation_.y -= fallSpeed;
			if (ClearTransform_.translation_.y <= targetY) {
				ClearTransform_.translation_.y = targetY;
				bounceTimer_ = 60;
				bounceAmplitude_ = 1.0f;
			}
		} else if (bounceTimer_ > 0) {
			float t = (60 - bounceTimer_) / 10.0f;
			ClearTransform_.translation_.y = targetY + std::abs(std::sin(t)) * bounceAmplitude_;
			bounceAmplitude_ *= bounceDecay_;
			bounceTimer_--;
			if (bounceTimer_ <= 0 || bounceAmplitude_ < 0.01f) {
				isBounceFinished_ = true;
				ClearTransform_.translation_.y = targetY;
				swayTimer_ = 0;
			}
		}
	} else {
		swayTimer_++;

		// ✨ ゆらゆらアニメ
		ClearTransform_.rotation_.y = baseRotY + std::sin(swayTimer_ * 0.01f) * 0.2f;

		ClearTransform_.rotation_.x = std::sin(swayTimer_ * 0.015f) * 0.1f;

		ClearTransform_.translation_.y = targetY + std::sin(swayTimer_ * 0.03f) * 0.3f;
	}

	ClearTransform_.UpdateMatrix();

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
			sceneManager_->ChangeScene(SceneID::TitleScene);

			return;
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
		if (pos.x <= -bgSize_.x) {
			pos.x += bgSize_.x * 2;
		}
		BackGround_[i]->SetPosition(pos);
	}

	// === スコアのカウントアップ（イージング） ===
	{
		if (!isScoreComplete_) {

			int diff = finalScore_ - displayScore_;

			if (diff > 0) {
				// 徐々に遅くなる加速
				int add = (std::max)(1, diff / 10);

				displayScore_ += add;

				// ポヨンアニメ開始（毎回少し大きく）
				scoreAnimScale_ = 1.3f;

				// 到達判定
				if (displayScore_ >= finalScore_) {
					displayScore_ = finalScore_;
					isScoreComplete_ = true;

					// 光エフェクト開始
					shineAlpha_ = 1.0f;
					shineX_ = 500;
				}
			}
		}

		// ポヨンアニメの収束
		scoreAnimScale_ += (1.0f - scoreAnimScale_) * scoreAnimSpeed_;
	}
	//=============--光エフェクト==============
	if (shineAlpha_ > 0.0f) {
		shineX_ += 20;        // 右へ走らせる
		shineAlpha_ -= 0.05f; // フェードアウト
		if (shineAlpha_ < 0.0f)
			shineAlpha_ = 0.0f;

		shineSprite_->SetPosition({shineX_, 480});
		shineSprite_->SetColor({1, 1, 1, shineAlpha_});
	}
}

void ClearScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// === 2D（背景） ===
	KamataEngine::Sprite::PreDraw(commandList);

	// 背景を先に描画
	for (int i = 0; i < 2; i++) {
		if (BackGround_[i])
			BackGround_[i]->Draw();
	}

	KamataEngine::Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	// === 3D（GameOver文字） ===
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (ClearModel_) {
		ClearModel_->Draw(ClearTransform_, Camera_);
	}
	Model::PostDraw();

	// === 2D（文字UIなど前景） ===
	KamataEngine::Sprite::PreDraw(commandList);

	if (isClear_ && frameCount_ % 150 >= 30) {
		sprite_->Draw(); // 「PRESS SPACE」など
	}

	if (fadeSprite_)
		fadeSprite_->Draw();

	if (shineAlpha_ > 0.0f) {
		shineSprite_->Draw();
	}

	// === スコア描画 ===
	{
		int score = displayScore_;

		float baseX = 600;
		float y = 500;

		std::vector<int> digits;
		if (score == 0) {
			digits.push_back(0);
		} else {
			int s = score;
			while (s > 0) {
				digits.push_back(s % 10);
				s /= 10;
			}
			std::reverse(digits.begin(), digits.end());
		}

		// 元画像サイズ（60×80）
		float originalW = 80.0f;
		float originalH = 100.0f;

		// 拡大後のサイズ
		float digitWidth = originalW * scoreAnimScale_;
		float digitHeight = originalH * scoreAnimScale_;

		float totalWidth = digitWidth * digits.size();
		float x = baseX - totalWidth / 2;

		for (int d : digits) {
			numberSprites_[d]->SetPosition({x, y});
			numberSprites_[d]->SetAnchorPoint({0.5f, 0.5f});

			numberSprites_[d]->SetSize({digitWidth, digitHeight});

			numberSprites_[d]->Draw();
			x += digitWidth;
		}
	}

	KamataEngine::Sprite::PostDraw();
}

void ClearScene::SetFinalScore(int score) { finalScore_ = score; }
