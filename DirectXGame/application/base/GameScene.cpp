#define NOMINMAX
#define NOMINMIN
#include "GameScene.h"
#include "../../DirectXGame/etc/MathUtilityForText.h"
#include <base/TextureManager.h>
#include <cassert>

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete skydome_;
	delete modelSkydome_;
	delete modelBlock_;
	delete cameraManager_;
	delete deathParticleModel_;
	delete enemyManager_;
	delete phaseManager_;

	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();

	delete mapManager_;
}

void GameScene::Initialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	camera_.Initialize();
	camera_.farZ = 600;

	//----------------------------マネージャー系統初期化----------------------------//

	// マップマネージャー初期化
	mapManager_ = new MapManager();
	mapManager_->Load("./Resources/blocks.csv");
	worldTransformBlocks_ = mapManager_->GenerateBlockTransforms(MapChipType::kBlock);

	// 敵マネージャ初期化
	EnemyModel_ = Model::CreateFromOBJ("Mushroom", true);
	enemyManager_ = new EnemyManager();
	enemyManager_->Initialize(EnemyModel_, &camera_);

	//----------------------------プレイヤー関係初期化----------------------------//
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	player_ = new Player();
	Vector3 playerPosition = mapManager_->GetPlayerSpawnPos();
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapManager_->GetMapChipField());

	//----------------------------プレイヤー関係初期化----------------------------//

	//----------------------------カメラコントローラー関係初期化----------------------------//

	cameraManager_ = new CameraManager();
	cameraManager_->Initialize(&camera_, player_);

	Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraManager_->GetViewProjection();

	//----------------------------カメラコントローラー関係初期化----------------------------//

	phaseManager_ = new PhaseManager();
	phaseManager_->Initialize(player_, enemyManager_, skydome_, cameraManager_, &worldTransformBlocks_, mapManager_->GetMapChipField());

	//----------------------------マネージャー系統初期化----------------------------//

	model_ = Model::Create();
	// textureHandle_ = TextureManager::Load("./Resources/player/player.png");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	modelBlock_ = Model::CreateFromOBJ("block", true);

	// ----------------------------フェード初期化---------------------------- //
	uint32_t blackTex = TextureManager::Load("./Resources/Title/fadeTexture.png");
	blackSprite_ = Sprite::Create(blackTex, {640, 360});
	blackSprite_->SetAnchorPoint({0.5f, 0.5f});
	blackSprite_->SetSize({1280.0f, 720.0f});
	blackSprite_->SetColor({0, 0, 0, 1}); // 真っ黒固定

	isFadingIn_ = true;
	fadeScele_ = 4.0f;

	// Ready / Start テクスチャ
	uint32_t readyTex = TextureManager::Load("./Resources/UI/Ready.png");
	readySprite_ = Sprite::Create(readyTex, {640, 360});
	readySprite_->SetAnchorPoint({0.5f, 0.5f});
	readySprite_->SetSize({400, 200}); // 適当な初期サイズ

	uint32_t startTex = TextureManager::Load("./Resources/UI/Start.png");
	startSprite_ = Sprite::Create(startTex, {640, 360});
	startSprite_->SetAnchorPoint({0.5f, 0.5f});
	startSprite_->SetSize({400, 200});

	isFadingIn_ = false;
	isStarting_ = true; // スタート演出開始
	startTimer_ = 0;
	readyScale_ = 0.0f;
	readyAlpha_ = 0.0f;
	startScale_ = 0.0f;
	startAlpha_ = 0.0f;
}

void GameScene::Update() {

	// ----------------------------スタート演出---------------------------- //
	if (isStarting_) {
		startTimer_++;

		// Ready表示（0〜120フレーム）
		if (startTimer_ < 120) {
			float t = startTimer_ / 120.0f;
			readyScale_ = std::min(1.2f, t * 1.5f); // だんだん拡大
			readyAlpha_ = std::min(1.0f, t * 2.0f); // フェードイン

			// 徐々にフェードアウト（後半）
			if (startTimer_ > 90) {
				readyAlpha_ = std::max(0.0f, 1.0f - (startTimer_ - 90) / 30.0f);
			}
		}
		// Start表示（120〜200フレーム）
		else if (startTimer_ >= 120 && startTimer_ < 200) {
			float t = (startTimer_ - 120) / 80.0f;
			startScale_ = std::min(1.3f, t * 1.6f);
			startAlpha_ = std::min(1.0f, t * 2.0f);

			if (startTimer_ > 170) {
				startAlpha_ = std::max(0.0f, 1.0f - (startTimer_ - 170) / 30.0f);
			}
		}
		// 終了
		else if (startTimer_ >= 200) {
			isStarting_ = false;
			isGameActive_ = true; // ← ゲーム開始
		}

		return; // スタート演出中はゲームロジック停止
	}

	// ----------------------------ゲーム本編処理---------------------------- //
	if (isGameActive_) {
		phaseManager_->Update();
		cameraManager_->Update();
		cameraManager_->TransferMatrix();

		if (enemyManager_->IsAllEnemyDefeated()) {
			nextScene_ = SceneID::Clear;
		}
		if (player_->IsDead()) {
			nextScene_ = SceneID::GameOver;
		}

		if (nextScene_ != SceneID::None) {
			sceneManager_->ChangeScene(nextScene_);
			nextScene_ = SceneID::None;
		}
	}
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);

	if (!player_->IsDead()) {
		player_->Draw();
	}
	skydome_->Draw();
	enemyManager_->Draw();

	for (auto& line : worldTransformBlocks_) {
		for (WorldTransform* block : line) {
			if (block)
				modelBlock_->Draw(*block, camera_);
		}
	}

	phaseManager_->Draw();

	Model::PostDraw();

	// ----------------------------フェード描画---------------------------- //
	if (isFadingIn_) {
		Sprite::PreDraw(commandList);
		blackSprite_->Draw();
		Sprite::PostDraw();
	}

	// ----------------------------スタート演出描画---------------------------- //
	if (isStarting_) {
		Sprite::PreDraw(commandList);

		// 黒背景を最初に描画（固定）
		blackSprite_->Draw();

		// Ready / Start の文字だけをアルファでフェード
		if (startTimer_ < 120) {
			readySprite_->SetSize({400 * readyScale_, 200 * readyScale_});
			readySprite_->SetColor({1, 1, 1, readyAlpha_}); // アルファ値でフェード
			readySprite_->Draw();
		} else if (startTimer_ < 200) {
			startSprite_->SetSize({400 * startScale_, 200 * startScale_});
			startSprite_->SetColor({1, 1, 1, startAlpha_});
			startSprite_->Draw();
		}

		Sprite::PostDraw();
	}
}