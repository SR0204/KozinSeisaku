#define NOMINMAX
#define NOMINMIN
#include "GameScene.h"
#include "../../DirectXGame/etc/MathUtilityForText.h"
#include <algorithm>
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
	phaseManager_->Initialize(player_, enemyManager_, skydome_, cameraManager_, &worldTransformBlocks_, mapManager_->GetMapChipField(), sceneManager_);

	//----------------------------マネージャー系統初期化----------------------------//

	model_ = Model::Create();
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

	// ----------------------------カウントダウンUI---------------------------- //
	uint32_t Count1Tex = TextureManager::Load("./Resources/UI/Count1.png");
	Count1Sprite_ = Sprite::Create(Count1Tex, {640, 360});
	Count1Sprite_->SetAnchorPoint({0.5f, 0.5f});
	Count1Sprite_->SetSize({400, 200});

	uint32_t Count2Tex = TextureManager::Load("./Resources/UI/Count2.png");
	Count2Sprite_ = Sprite::Create(Count2Tex, {640, 360});
	Count2Sprite_->SetAnchorPoint({0.5f, 0.5f});
	Count2Sprite_->SetSize({400, 200});

	uint32_t Count3Tex = TextureManager::Load("./Resources/UI/Count3.png");
	Count3Sprite_ = Sprite::Create(Count3Tex, {640, 360});
	Count3Sprite_->SetAnchorPoint({0.5f, 0.5f});
	Count3Sprite_->SetSize({400, 200});

	uint32_t StartTex = TextureManager::Load("./Resources/UI/Start.png");
	startSprite_ = Sprite::Create(StartTex, {640, 360});
	startSprite_->SetAnchorPoint({0.5f, 0.5f});
	startSprite_->SetSize({400, 200});

	isStarting_ = true;
	startTimer_ = 0;
}

void GameScene::Update() {

	// ----------------------------スリーカウント演出---------------------------- //
	if (isStarting_) {
		startTimer_++;

		const int kFramePerCount = 60;              // 1つの数字の表示時間（60フレーム = 1秒）
		const int totalFrames = kFramePerCount * 4; // 3,2,1,START の合計

		// 1秒ごとに数字を切り替える
		if (startTimer_ < totalFrames) {
			// スケールとアルファのフェード
			float localTime = (float)(startTimer_ % kFramePerCount) / kFramePerCount;
			float scale = 1.0f + 0.5f * (1.0f - localTime); // 出始め拡大、消えるとき縮小
			float alpha = 1.0f;
			if (localTime > 0.8f)
				alpha = 1.0f - (localTime - 0.8f) * 5.0f; // フェードアウト

			currentScale_ = scale;
			currentAlpha_ = std::clamp(alpha, 0.0f, 1.0f);

			// カウント番号を決定
			currentCount_ = 3 - (startTimer_ / kFramePerCount);
			if (currentCount_ < 0)
				currentCount_ = 0; // 最後は "START!"
		} else {
			// 終了してゲームスタート
			isStarting_ = false;
			isGameActive_ = true;
		}

		// ここで return する！ → カウント中は本編を止める

		return;
	}

	// ----------------------------ゲーム本編処理---------------------------- //
	if (isGameActive_) {
		// --- フェーズ更新 ---
		auto nextScene = phaseManager_->Update();

		cameraManager_->Update();
		cameraManager_->TransferMatrix();

		// --- 全敵撃破処理 ---
		if (enemyManager_->IsAllEnemyDefeated()) {
			nextScene_ = SceneID::Clear;
		}

		// --- 死亡演出終了で返ってきたら遷移 ---
		if (nextScene.has_value()) {
			sceneManager_->ChangeScene(nextScene.value());
			return;
		}

		if (nextScene_ != SceneID::None) {
			sceneManager_->ChangeScene(nextScene_);
			nextScene_ = SceneID::None;
		}
	}
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// モデル描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(Model::CullingMode::kNone, Model::BlendMode::kNormal, Model::DepthTestMode::kOn);
	if (!player_->IsDead())
		player_->Draw();
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

	// ----------------------------スリーカウント描画---------------------------- //

	if (isStarting_) {
		Sprite::PreDraw(commandList);
		blackSprite_->Draw();
		Sprite* drawSprite = nullptr;
		if (currentCount_ == 3)
			drawSprite = Count3Sprite_;
		else if (currentCount_ == 2)
			drawSprite = Count2Sprite_;
		else if (currentCount_ == 1)
			drawSprite = Count1Sprite_;
		else
			drawSprite = startSprite_;

		drawSprite->SetSize({800 * currentScale_, 400 * currentScale_});
		drawSprite->SetColor({1, 1, 1, currentAlpha_});
		drawSprite->Draw();

		Sprite::PostDraw();
	}
}
