#define NOMINMAX
#include "StageSelect.h"
#include "../SceneManager/SceneID.h"
#include "engine/Scene/SceneManager/SceneManager.h"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

StageSelect::StageSelect() {}

StageSelect::~StageSelect() {
	for (int i = 0; i < 2; i++) {
		delete BackGround_[i].release();
	}
	for (int i = 0; i < kMaxStage; i++) {
		delete stageCubeModel_[i].release();
		delete stageTextModel_[i].release();
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
	bgTexHandle_ = TextureManager::Load("./Resources/Title/TitleBack.png");
	BackGround_[0].reset(Sprite::Create(bgTexHandle_, {0, 0}));
	BackGround_[1].reset(Sprite::Create(bgTexHandle_, {1280, 0}));

	//==========ステージアイコン==============
	for (int i = 0; i < kMaxStage; i++) {
		std::string base = "stage_" + std::to_string(i + 1);

		stageCubeModel_[i].reset(Model::CreateFromOBJ(base + "_Cube", true));

		stageTransform_[i].Initialize();
		float x = (i - (kMaxStage - 1) / 2.0f) * 8.0f;
		stageTransform_[i].translation_ = {x, 0.0f, 0.0f};
		stageTransform_[i].scale_ = {0.8f, 0.8f, 0.8f};

		assert(stageCubeModel_[0] && "stage_1_Cube load failed");
	}

	float stageScale = 0.5f; // ステージアイコンのサイズ倍率

	Vector2 iconBaseSize = {500.0f * stageScale, 500.0f * stageScale};

	// === カーソル ===
	cursorTexHandle_ = TextureManager::Load("./Resources/Stage/cursor.png");
	cursorSprite_.reset(Sprite::Create(cursorTexHandle_, {0, 0}));

	float cursorScale = 0.5f; // ← カーソルの大きさ調整
	cursorSprite_->SetSize({128.0f * cursorScale, 128.0f * cursorScale});

	// 初期位置をステージ0に合わせる
	Vector3 pos = stageTransform_[currentStage_].translation_;
	cursorSprite_->SetPosition({640 + pos.x * 50.0f, 360 - pos.y * 50.0f});

	// === フェード ===
	fadeTexHandle_ = TextureManager::Load("./Resources/Title/fadeTexture.png");
	fadeSprite_.reset(Sprite::Create(fadeTexHandle_, {0, 0}));
	fadeSprite_->SetSize({1280, 720});
	fadeSprite_->SetColor({0, 0, 0, 0});

	// === モデル（中央に飾り3D） ===
	model_.reset(KamataEngine::Model::CreateFromOBJ("SelectModel", true));
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = {0, 5, 0};
	worldTransform_.rotation_.y = 0.0f;

	camera_.Initialize();

	// === ライト設定 ===
	lightGroup_.reset(LightGroup::Create());
	/*lightGroup_->SetDirLightDir(0, {0.3f, -1.0f, 0.4f});
	lightGroup_->SetDirLightColor(0, {0.6f, 0.2f, 0.2f});
	lightGroup_->SetAmbientColor({0.2f, 0.05f, 0.05f});*/

	for (int i = 0; i < kMaxStage; i++) {
		stageCubeModel_[i]->SetLightGroup(lightGroup_.get());
	}

	// === BGM ===
	// bgmHandle_ = Audio::GetInstance()->LoadWave("./Resources/Sound/StageSelectBGM.mp3");
	// bgmVoiceHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	// bgmVolume_ = 0.5f;
	// Audio::GetInstance()->SetVolume(bgmVoiceHandle_, bgmVolume_);

	currentStage_ = 0;
	isFadingOut_ = false;
	isDecide_ = false;
	fadeAlpha_ = 0.0f;

	for (int i = 0; i < kMaxStage; i++) {
		baseScale_[i] = {0.7f, 0.7f, 0.7f};
	}
}

void StageSelect::Update() {
	frameCount_++;

	// === 入力 ===
	if (!isDecide_) {
		// ←左右入力でステージ選択
		if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerKey(XINPUT_GAMEPAD_DPAD_RIGHT)) {
			currentStage_ = std::min(currentStage_ + 1, kMaxStage - 1);
		}
		if (input_->TriggerKey(DIK_LEFT) || input_->TriggerKey(XINPUT_GAMEPAD_DPAD_LEFT)) {
			currentStage_ = std::max(currentStage_ - 1, 0);
		}

		// 決定
		if (input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_RETURN)) {
			isDecide_ = true;
			isStageRotating_ = true;
			rotateTimer_ = 0;
		}

		// タイトルに戻る
		if (input_->TriggerKey(DIK_ESCAPE)) {
			isFadingOut_ = true;
			isDecide_ = false;
		}
	}

	//================選択中に少し浮くようにする===========================
	for (int i = 0; i < kMaxStage; i++) {
		if (i == currentStage_) {
			stageTransform_[i].scale_ = baseScale_[i];
			stageTransform_[i].translation_.y = std::sin(frameCount_ * 0.05f) * 0.3f;
		} else {
			stageTransform_[i].scale_ = baseScale_[i];
			stageTransform_[i].translation_.y = 0.0f;
		}

		stageTransform_[i].UpdateMatrix();
	}

	if (isStageRotating_) {
		rotateTimer_++;

		// 1回転（2π）
		float rot = std::numbers::pi_v<float> * 2.0f * (rotateTimer_ / 60.0f);

		stageTransform_[currentStage_].rotation_.y = rot;

		if (rotateTimer_ >= 60) {
			isStageRotating_ = false;
			isFadingOut_ = true; // 回転後にフェード
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

	// ステージアイコン描画
	for (int i = 0; i < kMaxStage; i++) {
		stageCubeModel_[i]->Draw(stageTransform_[i], camera_);
	}

	Model::PostDraw();

	// === 2D ===
	Sprite::PreDraw(commandList);

	// 背景
	for (int i = 0; i < 2; i++) {
		if (BackGround_[i])
			BackGround_[i]->Draw();
	}

	// カーソル（点滅演出）
	if (frameCount_ % 60 < 40 && cursorSprite_) {
		cursorSprite_->Draw();
	}

	// フェード
	if (fadeSprite_)
		fadeSprite_->Draw();

	Sprite::PostDraw();

	ImGuiManager::GetInstance()->Draw();
	DrawImGui();
	ImGuiManager::GetInstance()->End();
}

void StageSelect::DrawImGui() {

#ifdef USE_IMGUI
	ImGui::Begin("Stage Icon Adjust");

	// --- ステージ番号選択 ---
	ImGui::SliderInt("Stage Index", &debugStageIndex_, 0, kMaxStage - 1);

	if (0 <= debugStageIndex_ && debugStageIndex_ < kMaxStage) {

		// ===== Position =====
		Vector3& pos = stageTransform_[debugStageIndex_].translation_;
		float posBuf[3] = {pos.x, pos.y, pos.z};

		if (ImGui::DragFloat3("Stage Position", posBuf, 0.1f)) {
			pos.x = posBuf[0];
			pos.y = posBuf[1];
			pos.z = posBuf[2];
			stageTransform_[debugStageIndex_].UpdateMatrix();
		}

		// ===== Rotation =====
		Vector3& rot = stageTransform_[debugStageIndex_].rotation_;
		float rotBuf[3] = {rot.x, rot.y, rot.z};

		if (ImGui::DragFloat3("Stage Rotation", rotBuf, 0.01f)) {
			rot.x = rotBuf[0];
			rot.y = rotBuf[1];
			rot.z = rotBuf[2];
			stageTransform_[debugStageIndex_].UpdateMatrix();
		}

		// ===== Scale =====
		Vector3& scl = baseScale_[debugStageIndex_];
		float sclBuf[3] = {scl.x, scl.y, scl.z};

		if (ImGui::DragFloat3("Stage Base Scale", sclBuf, 0.01f)) {
			scl.x = sclBuf[0];
			scl.y = sclBuf[1];
			scl.z = sclBuf[2];
		}
	}

	ImGui::End();

	// ===== Camera =====
	ImGui::Begin("Camera");

	{
		float camPos[3] = {camera_.translation_.x, camera_.translation_.y, camera_.translation_.z};
		if (ImGui::DragFloat3("Camera Pos", camPos, 0.1f)) {
			camera_.translation_.x = camPos[0];
			camera_.translation_.y = camPos[1];
			camera_.translation_.z = camPos[2];
		}

		float camRot[3] = {camera_.rotation_.x, camera_.rotation_.y, camera_.rotation_.z};
		if (ImGui::DragFloat3("Camera Rot", camRot, 0.01f)) {
			camera_.rotation_.x = camRot[0];
			camera_.rotation_.y = camRot[1];
			camera_.rotation_.z = camRot[2];
		}
	}

	ImGui::End();
#endif
}
