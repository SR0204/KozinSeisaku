#include "GameScene.h"
#include "CameraController.h"
#include "MathUtilityForText.h"
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

	delete sprite_;
	delete sprite2_;
}

void GameScene::Initialize() {
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
	EnemyModel_ = Model::CreateFromOBJ("enemy", true);
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

	phase_ = Phase::kTitle;

	model_ = Model::Create();
	textureHandle_ = TextureManager::Load("./Resources/player/player.png");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	modelBlock_ = Model::CreateFromOBJ("block", true);

	// ファイル名を指定してテクスチャを読み込む
	TitleTextureHandle_ = TextureManager::Load("./Resources/Title/TitleKey.png");

	TitleTextureHandle2_ = TextureManager::Load("./Resources/Title/Title.png");

	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(TitleTextureHandle_, {0, 0});

	sprite2_ = Sprite::Create(TitleTextureHandle2_, {0, 0});
}

void GameScene::Update() {

	frameCount++;

	if (isTitle) {
		if (isTitle && Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			isTitle = false;

			isBackgroundStarted_ = true;
		}
		// sin波で上下に揺れるY座標を作る（±10ピクセル範囲で動かす）
		float y = 10 * sin(frameCount * 0.05f);
		// スプライトの位置を更新
		sprite_->SetPosition({0.0f, y});
	}
	// sprite2_ の移動処理（上から下へ）
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

	// フェーズマネージャー更新
	phaseManager_->Update();

	// #ifdef _DEBUG
	//	if (input_->TriggerKey(DIK_SPACE)) {
	//		cameraManager_->SetDebugCameraActive(true);
	//	}
	// #endif

	cameraManager_->Update();
	cameraManager_->TransferMatrix();
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	// スプライトインスタンスの描画処理
	if (phase_ == Phase::kTitle && isTitle && frameCount % 150 >= 30) {
		sprite_->Draw();
	}
	if (phase_ == Phase::kTitle && isTitle) {
		sprite2_->Draw();
	}
	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);

	if (isBackgroundStarted_ == true) {
		player_->Draw();
		skydome_->Draw();

		enemyManager_->Draw();

		for (auto& line : worldTransformBlocks_) {
			for (WorldTransform* block : line) {
				if (block)
					modelBlock_->Draw(*block, camera_);
			}
		}
	}

	phaseManager_->Draw();

	Model::PostDraw();
}