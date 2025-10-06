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
	fadeSprite_ = Sprite::Create(blackTex, {640, 360});
	fadeSprite_->SetAnchorPoint({0.5f, 0.5f});
	fadeSprite_->SetSize({1280.0f, 720.0f});

	isFadingIn_ = true;
	fadeScele_ = 4.0f;

	uint32_t readyTex = TextureManager::Load("./Resources/UI/Ready.png");
	readySprite_ = Sprite::Create(readyTex, {640, 360});
	readySprite_->SetAnchorPoint({0.5f, 0.5f});

	uint32_t startTex = TextureManager::Load("./Resources/UI/Start.png");
	startSprite_ = Sprite::Create(startTex, {640, 360});
	startSprite_->SetAnchorPoint({0.5f, 0.5f});
}

void GameScene::Update() {

	// ----------------------------フェード処理---------------------------- //
	if (isFadingIn_) {
		fadeScele_ -= 0.05f;
		fadeSprite_->SetSize({1280.0f * fadeScele_, 720.0f * fadeScele_});

		if (fadeScele_ <= 0.1f) {
			isFadingIn_ = false;
			isStarting_ = true; // ← スタート演出を開始
			startTimer_ = 0;
		}
		return; // フェード中は他の更新を止める
	}

	// ----------------------------スタート演出処理---------------------------- //
	if (isStarting_) {
		startTimer_++;

		// 60フレーム = 約1秒で「Ready」
		// 120フレーム = 約2秒で「Start」
		// 180フレーム以降にゲーム開始
		if (startTimer_ == 60) {
			// Ready表示を出すならここ
		} else if (startTimer_ == 120) {
			// Start表示を出すならここ
		} else if (startTimer_ >= 180) {
			isStarting_ = false;
			isGameActive_ = true; // ← ここからプレイヤー・敵が動く
		}
		return; // 演出中はゲームロジックを止める
	}

	// ----------------------------ゲーム本編処理---------------------------- //
	if (isGameActive_) {

		phaseManager_->Update(); // 敵・プレイヤー等を動かす
		cameraManager_->Update();
		cameraManager_->TransferMatrix();

		// --- クリア・ゲームオーバー判定 ---
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

	// 常に描画
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
		fadeSprite_->Draw();
		Sprite::PostDraw();
	}

	// スタート演出中
	if (isStarting_) {
		Sprite::PreDraw(commandList);

		if (startTimer_ < 120) {
			readySprite_->Draw();
		} else if (startTimer_ < 180) {
			startSprite_->Draw();
		}

		Sprite::PostDraw();
	}
}
