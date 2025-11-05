#include "StageSelect.h"
#include "../SceneManager/SceneID.h"
#include "engine/Scene/SceneManager/SceneManager.h"
#include <cmath>

using namespace KamataEngine;

StageSelect::StageSelect() {}

StageSelect::~StageSelect() {
	for (int i = 0; i < 2; i++) {
		delete BackGround_[i].release();
	}
	for (int i = 0; i < kMaxStage; i++) {
		delete stageSprite_[i].release();
	}
	delete cursorSprite_.release();
	delete fadeSprite_.release();
	delete model_.release();

	if (bgmVoiceHandle_ != -1) {
		Audio::GetInstance()->StopWave(bgmVoiceHandle_);
	}
}

void StageSelect::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	// === 背景 ===
	bgTexHandle_ = TextureManager::Load("./Resources/StageSelect/stage_bg.png");
	BackGround_[0].reset(Sprite::Create(bgTexHandle_, {0, 0}));
	BackGround_[1].reset(Sprite::Create(bgTexHandle_, {1280, 0}));

	// === ステージアイコン ===
	stageTex_[0] = TextureManager::Load("./Resources/StageSelect/stage1.png");
	stageTex_[1] = TextureManager::Load("./Resources/StageSelect/stage2.png");
	stageTex_[2] = TextureManager::Load("./Resources/StageSelect/stage3.png");

	for (int i = 0; i < kMaxStage; i++) {
		stageSprite_[i].reset(Sprite::Create(stageTex_[i], {200.0f + i * 300.0f, 300.0f}));
	}

	// === カーソル ===
	cursorTexHandle_ = TextureManager::Load("./Resources/StageSelect/cursor.png");
	cursorSprite_.reset(Sprite::Create(cursorTexHandle_, {200.0f, 500.0f}));

	// === フェード ===
	fadeTexHandle_ = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_.reset(Sprite::Create(fadeTexHandle_, {0, 0}));
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({0, 0, 0, 0});

	// === モデル（中央に飾り3D） ===
	model_.reset(Model::CreateFromOBJ("SelectModel", true));
	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f};
	worldTransform_.translation_ = {0, 5, 0};
	worldTransform_.rotation_.y = 0.0f;

	camera_.Initialize();

	// === ライト設定 ===
	lightGroup_.reset(LightGroup::Create());
	lightGroup_->SetDirLightDir(0, {0.3f, -1.0f, 0.4f});
	lightGroup_->SetDirLightColor(0, {1.3f, 1.2f, 1.1f});
	model_->SetLightGroup(lightGroup_.get());

	// === BGM ===
	bgmHandle_ = Audio::GetInstance()->LoadWave("./Resources/Sound/StageSelectBGM.mp3");
	bgmVoiceHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	bgmVolume_ = 0.5f;
	Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);

	currentStage_ = 0;
	isFadingOut_ = false;
	isDecide_ = false;
	fadeAlpha_ = 0.0f;
}

void StageSelect::Update() {
	frameCount_++;

	// === 入力 ===
	if (!isDecide_) {
		if (input_->TriggerKey(DIK_LEFT)) {
			currentStage_--;
			if (currentStage_ < 0)
				currentStage_ = kMaxStage - 1;
		}
		if (input_->TriggerKey(DIK_RIGHT)) {
			currentStage_++;
			if (currentStage_ >= kMaxStage)
				currentStage_ = 0;
		}

		// カーソル移動
		Vector2 pos = {200.0f + currentStage_ * 300.0f, 500.0f};
		cursorSprite_->SetPosition(pos);

		// 決定
		if (input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_RETURN)) {
			isDecide_ = true;
			isFadingOut_ = true;
		}

		// タイトルに戻る
		if (input_->TriggerKey(DIK_ESCAPE)) {
			isFadingOut_ = true;
			isDecide_ = false;
		}
	}

	// === モデルの演出（回転） ===
	worldTransform_.rotation_.y += 0.01f;
	worldTransform_.UpdateMatrix();

	// === 背景スクロール ===
	for (int i = 0; i < 2; i++) {
		Vector2 pos = BackGround_[i]->GetPosition();
		pos.x -= bgScrollSpeed_;
		if (pos.x <= -1280)
			pos.x += 1280 * 2;
		BackGround_[i]->SetPosition(pos);
	}

	// === フェードアウト処理 ===
	if (isFadingOut_) {
		bgmVolume_ -= 0.02f;
		if (bgmVolume_ < 0.0f)
			bgmVolume_ = 0.0f;
		Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);

		fadeAlpha_ += 0.02f;
		if (fadeAlpha_ > 1.0f)
			fadeAlpha_ = 1.0f;
		fadeSprite_->SetColor({0, 0, 0, fadeAlpha_});

		if (fadeAlpha_ >= 1.0f) {
			Audio::GetInstance()->StopWave(bgmVoiceHandle_);

			if (isDecide_) {
				// ここで選択中ステージをSceneManagerに渡す
				sceneManager_->SetSelectedStage(currentStage_);

				// そしてGameSceneへ
				sceneManager_->ChangeScene(SceneID::Game);
			} else {
				// Escで戻った場合
				sceneManager_->ChangeScene(SceneID::TitleScene);
			}
			return;
		}
	}

	// === ライトアニメーション ===
	Vector3 lightDir;
	lightDir.x = std::cos(frameCount_ * 0.01f) * 0.5f;
	lightDir.y = -1.0f;
	lightDir.z = std::sin(frameCount_ * 0.01f) * 0.5f;
	lightGroup_->SetDirLightDir(0, lightDir);
	lightGroup_->Update();
}

void StageSelect::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// === 3D ===
	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (model_) {
		model_->Draw(worldTransform_, camera_);
	}
	Model::PostDraw();

	// === 2D ===
	Sprite::PreDraw(commandList);

	// 背景
	for (int i = 0; i < 2; i++) {
		if (BackGround_[i])
			BackGround_[i]->Draw();
	}

	// ステージアイコン描画
	for (int i = 0; i < kMaxStage; i++) {
		if (stageSprite_[i])
			stageSprite_[i]->Draw();
	}

	// カーソル（点滅演出）
	if (frameCount_ % 60 < 40 && cursorSprite_) {
		cursorSprite_->Draw();
	}

	// フェード
	if (fadeSprite_)
		fadeSprite_->Draw();

	Sprite::PostDraw();
}
