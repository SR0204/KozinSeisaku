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

	delete cameraController_;

	// delete enemy_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	// 解放
	/*for (Enemy* enemy : enemies_) {
		delete enemy;
	}*/

	// worldTransformBlocks_.clear();

	delete debugCamera_;

	// マップチップフィールドの開放
	delete mapChipField_;

	delete deathParticleModel_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクション生成
	camera_.Initialize();
	camera_.farZ = 600; // これを５００とかにすると後ろの方ででっかい弾が出る。

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 敵初期化
	EnemyModel_ = Model::CreateFromOBJ("enemy", true);

	// Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(17, 18);

	// enemy_ = new Enemy();

	// enemy_->Initialize(EnemyModel_, &viewProjection_, enemyPosition);

	// 仮の生成処理。後で消す

	/*Vector3 DeathParticlePosition = mapChipField_->GetMapChipPositionByIndex(3, 18);

	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(deathParticleModel_, &viewProjection_, DeathParticlePosition);*/

	// 敵の生成
	/*for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = {10 + i * 5.0f, 5, 0};
		newEnemy->Initialize(EnemyModel_, &viewProjection_, enemyPosition);

		enemies_.push_back(newEnemy);
	}*/

	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;

	// 表示ブロックの生成
	GenerateBlocks();

	// 3Dモデルデータの生成
	model_ = Model::Create();

	// 画像の読み込み
	textureHandle_ = TextureManager::Load("player.png");

	// ３Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// プレイヤーモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// プレイヤーの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 18);

	// プレイヤーの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition); // 元player_->Initialize(modelPlayer_, &viewProjection_);

	player_->SetMapChipField(mapChipField_);

	// カメラコントローラーの初期化
	cameraController_ = new CameraController;
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	// 更新
	cameraController_->Reset();

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// ブロックのモデルを読み込む
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
}

void GameScene::Update() {

	ChangePhase();

	// デバッグカメラの更新
	debugCamera_->Update();

#ifdef _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}

	player_->Update();

#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		camera_.TransferMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// --- 背景スプライト ---
	Sprite::PreDraw(commandList);



	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	// --- 3D描画 ---
	KamataEngine::ModelCommon::PipelineSetKey pipelineKey{};
	pipelineKey.cullingMode = KamataEngine::ModelCommon::CullingMode::kBack;
	pipelineKey.blendMode = KamataEngine::ModelCommon::BlendMode::kNormal;
	pipelineKey.depthTestMode = KamataEngine::ModelCommon::DepthTestMode::kOn;

	// モデル共通でcommandListをセット
	KamataEngine::ModelCommon::GetInstance()->PreDraw(pipelineKey, commandList);

	// モデル側の描画設定（省略可）
	KamataEngine::Model::PreDraw();
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// プレイヤーの描画処理
	if (phase_ == Phase::kPlay) {
		player_->Draw(commandList);
	}

	// 天球の描画処理
	skydome_->Draw();

	// 敵の描画処理
	// enemy_->Draw();

	/*for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}*/

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {

	// 要素数
	const uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	// ブロック1個分の横幅
	// const float kBlockWidth = 2.0f;
	// const float kBlockHeight = 2.0f;
	// 要素数を変更する
	// 配列を設定
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		// １列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions() {

//	// 自キャラと敵キャラの当たり判定
//#pragma region
//
//	// 判定対象１と２の座標(宣言)
//	AABB aabb1, aabb2;
//
//	// 自キャラの座標
//	aabb1 = player_->GetAABB();
//
//	for (Enemy* enemy : enemies_) {
//		// 敵弾の座標
//		aabb2 = enemy->GetAABB();
//
//		// AABB同士の交差判定
//		if (IsColision(aabb1, aabb2)) {
//			// 自キャラの衝突時コールバックを呼び出す
//			player_->OnCollision(enemy);
//			// 敵弾の衝突時コールバックを呼び出す
//			enemy->OnCollision(player_);
//		}
//	}

#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {

	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		//   天球の更新
		skydome_->Update();

		// プレイヤーの更新
		player_->Update();

		// 敵の更新
		// enemy_->Update();
		//
		// 敵の更新
		/*for (Enemy* enemy : enemies_) {
			enemy->Update();
		}*/

		// カメラコントローラーの更新
		cameraController_->Update();

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock)
					continue;

				//// 平行移動行列
				// Matrix4x4 result = {
				//     1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, worldTransformBlock->translation_.x, worldTransformBlock->translation_.y,
				//     worldTransformBlock->translation_.z, 1.0f};

				// Matrix4x4 matWorld = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				//// 平行移動だけ代入
				// worldTransformBlock->matWorld_ = matWorld;

				//// 定数バッファに転送する
				// worldTransformBlock->TransferMatrix();

				worldTransformBlock->UpdateMatrix();
			}
		}

		//isDead_ = player_->IsDead();

		//if (isDead_ == true) {

		//	// 死亡演出フェーズに切り替え
		//	phase_ = Phase::kDeath;

		//	// 自キャラの座標を取得
		//	const Vector3& deathParticlesPosition = player_->GetWorldPosition();

		//	// デスパーティクル初期化
		//	// Vector3 DeathParticlePosition = mapChipField_->GetMapChipPositionByIndex(3, 18);

		//	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

		//	deathParticles_ = new DeathParticles;
		//	deathParticles_->Initialize(deathParticleModel_, &viewProjection_, deathParticlesPosition);
		//}

		// 全ての当たり判定を行う
		CheckAllCollisions();

		break;

	case Phase::kDeath:

		//  天球の更新
		skydome_->Update();

		// 敵の更新
		/*for (Enemy* enemy : enemies_) {
			enemy->Update();
		}*/

		// デスパーティクル
		//deathParticles_->Update();

		// カメラコントローラーの更新
		cameraController_->Update();

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock)
					continue;

				//// 平行移動行列
				// Matrix4x4 result = {
				//     1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, worldTransformBlock->translation_.x, worldTransformBlock->translation_.y,
				//     worldTransformBlock->translation_.z, 1.0f};

				// Matrix4x4 matWorld = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				//// 平行移動だけ代入
				// worldTransformBlock->matWorld_ = matWorld;

				//// 定数バッファに転送する
				// worldTransformBlock->TransferMatrix();

				worldTransformBlock->UpdateMatrix();
			}
		}

		/*if (deathParticles_ && deathParticles_->IsFinished()) {

			finished_ = true;
		}*/

		break;
	}
}