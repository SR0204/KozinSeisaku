#include "GameScene.h"
#include "../../MathUtilityForText.h"
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

	model_ = Model::Create();
	textureHandle_ = TextureManager::Load("./Resources/player/player.png");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	modelBlock_ = Model::CreateFromOBJ("block", true);
}

void GameScene::Update() {

	// フェーズマネージャー更新
	phaseManager_->Update();

	cameraManager_->Update();
	cameraManager_->TransferMatrix();

	// クリア判定
	if (enemyManager_->IsAllEnemyDefeated()) {
		nextScene_ = SceneID::Clear; // フラグを立てるだけ
	}

	// ゲームオーバー判定
	if (player_->IsDead()) {
		nextScene_ = SceneID::GameOver; // フラグを立てるだけ
	}

	// Update の最後で切り替え
	if (nextScene_ != SceneID::None) {
		sceneManager_->ChangeScene(nextScene_);
		nextScene_ = SceneID::None; // フラグをリセット
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
}
