#define NOMINMAX
#include "EnemyManager.h"
#include "../../DirectXGame/etc/MathUtilityForText.h" // IsColision 用
#include "../../engine/Player/Player.h"
#include <algorithm>
#include <engine/Score/ScoreManager/ScoreManager.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace KamataEngine;

EnemyManager::EnemyManager() : enemyModel_(nullptr), camera_(nullptr) {}

EnemyManager::~EnemyManager() {
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
}

void EnemyManager::Initialize(Model* enemyModel, Camera* camera, MapChipField* mapField, const std::string& csvPath, int stageNo) {
	enemyModel_ = enemyModel;
	camera_ = camera;
	audio_ = Audio::GetInstance();

	auto enemyPositions = LoadEnemyPositionsFromCSV(csvPath, mapField);

	// ★ スポーンポイントとして保存
	spawnPoints_ = enemyPositions;

	// ★ 初期敵を生成（今まで通り）
	for (auto& pos : enemyPositions) {
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(enemyModel_, camera_, pos);
		enemies_.push_back(newEnemy);
	}

	enemyDeathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

	// ===== ステージ2：中ボス専用 =====
	if (stageNo == 2) {
		Vector3 bossPos = {10.0f, 5.0f, 0.0f};

		Enemy* midBoss = new Enemy();
		midBoss->InitializeMidBoss(enemyModel_, camera_, bossPos);
		midBoss->SetEnemyManager(this);
		enemies_.push_back(midBoss);

		return; // ★ 通常敵を出さない
	}
}

void EnemyManager::Update(MapChipField* mapField) {

	// ===== 敵の更新 =====
	for (Enemy* enemy : enemies_) {
		enemy->Update(mapField);
	}

	// ===== ここからスポーン処理 =====
	spawnTimer_ += 1.0f / 60.0f; // 毎フレーム加算（60FPS）

	if (spawnTimer_ >= spawnInterval_) {
		spawnTimer_ = 0.0f;

		if (!spawnPoints_.empty()) {
			// 乱数でスポーン地点を選ぶ
			int index = rand() % spawnPoints_.size();

			// 敵を生成
			SpawnEnemy(spawnPoints_[index]);
		}
	}

	// ===== デスパーティクル更新 =====
	for (auto it = deathParticles_.begin(); it != deathParticles_.end();) {
		(*it)->Update();
		if ((*it)->IsFinished()) {
			delete *it;
			it = deathParticles_.erase(it);
		} else {
			++it;
		}
	}

	// ====== ヒットストップ中なら処理を止める ======
	if (hitStopTime_ > 0.0f) {
		hitStopTime_ -= 1.0f / 60.0f;
		return; // 敵の更新を止める
	}
}

void EnemyManager::Draw() {
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	for (auto* p : deathParticles_) {
		p->Draw();
	}
}

void EnemyManager::CheckAllCollisions(Player* player) {
	// 1. プレイヤーのAABBを取得
	AABB playerAABB = player->GetAABB();

	// 2. 各敵と判定
	for (Enemy* enemy : enemies_) {
		if (!enemy->IsAlive())
			continue; // 既に倒れていたらスキップ

		if (IsColision(playerAABB, enemy->GetAABB())) {

			//=========================中ボス用=============================
			if (enemy->GetType() == EnemyType::MidBoss) {

				// 中ボスは消えない
				player->SetVelocityY(player->GetBouncePower() * 2.0f);

				if (score_) {
					score_->AddScore(500);
				}

				player->consecutiveBouncePoints_ = std::min(player->consecutiveBouncePoints_ + 1, Player::kMaxBouncePoints);

				return;
			}

			// 上から踏んだ判定
			if (player->GetVelocity().y < 0.0f && playerAABB.min.y > enemy->GetAABB().max.y - 1.0f) {

				// 踏んだ！即消し

				enemy->SetAlive(false);

				// ★ヒットストップ発生
				hitStopTime_ = kHitStopDuration_;

				// ★カメラシェイク（0.1秒・揺れ0.15）
				cameraManager_->StartShake(0.2f, 0.35f);

				player->SetVelocityY(0.25f); // 軽くバウンド

				// ----- ここでスコア加算 -----
				if (score_) {
					score_->AddScore(stompScore_); // ← Score クラスの加算関数に合わせて変更
				}

				// --- 効果音を再生 ---
				// Audio::GetInstance()->PlayWave(enemyDeathSE_);

				// ここでデスパーティクルを生成
				EnemyDeathParticles* p = new EnemyDeathParticles();
				p->Initialize(enemyDeathParticleModel_, camera_, enemy->GetWorldPosition());
				deathParticles_.push_back(p);

				//------------------------------------------
				// ★ 連続踏みポイントを増やす（最大5）
				player->consecutiveBouncePoints_ = std::min(player->consecutiveBouncePoints_ + 1, Player::kMaxBouncePoints);

				//------------------------------------------
				// ★ 高く飛ぶ処理（スペース押しっぱで強化）
				//------------------------------------------
				float bouncePower = player->GetBouncePower(); // 基本のバウンド力

				// スペース押しっぱ → 高く飛ぶ
				if (Input::GetInstance()->PushKey(DIK_SPACE)) {
					bouncePower *= 1.6f; // 高さ倍率（好きに調整OK）
				}

				// 上方向に跳ねさせる
				player->SetVelocityY(bouncePower);
				//------------------------------------------

			} else {
				// 横や下から当たった場合
				player->OnCollision(enemy);
			}
		}
	}

	// 3. 死んだ敵を削除
	enemies_.erase(
	    std::remove_if(
	        enemies_.begin(), enemies_.end(),
	        [](Enemy* e) {
		        if (!e->IsAlive()) {
			        delete e;
			        return true;
		        }
		        return false;
	        }),
	    enemies_.end());
}

void EnemyManager::HandleEnemyCollisions() {
	int n = (int)enemies_.size();
	for (int i = 0; i < n; ++i) {
		Enemy* e1 = enemies_[i];
		if (!e1->IsAlive())
			continue;

		for (int j = i + 1; j < n; ++j) {
			Enemy* e2 = enemies_[j];
			if (!e2->IsAlive())
				continue;

			if (IsColision(e1->GetAABB(), e2->GetAABB()) && e1->collisionCooldown_ == 0 && e2->collisionCooldown_ == 0) {

				e1->ReverseDirection();
				e2->ReverseDirection();

				// --- AABB の重なりを解消する ---
				AABB a = e1->GetAABB();
				AABB b = e2->GetAABB();

				// 横方向の重なり量を計算
				float overlapX1 = a.max.x - b.min.x;
				float overlapX2 = b.max.x - a.min.x;
				float overlapX = std::min(overlapX1, overlapX2);

				if (overlapX > 0) {
					// 双方を半分ずつ押し戻す
					e1->AddWorldX(-overlapX * 0.5f);
					e2->AddWorldX(overlapX * 0.5f);
				}

				// クールダウンを少し長めに
				e1->collisionCooldown_ = 10;
				e2->collisionCooldown_ = 10;
			}
		}
	}
}

bool EnemyManager::IsAllEnemyDefeated() const { return enemies_.empty(); }

std::vector<KamataEngine::Vector3> EnemyManager::LoadEnemyPositionsFromCSV(const std::string& filename, MapChipField* mapField) {
	std::vector<Vector3> positions;
	std::ifstream file(filename);
	if (!file.is_open())
		return positions;

	std::string line;
	int row = 0;

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string cell;
		int col = 0;

		while (std::getline(ss, cell, ',')) {
			int value = std::stoi(cell);

			if (value == 3) { // 「3」は敵
				// X/Z座標をブロックに合わせる
				float x = col * MapChipField::kBlockWidth + MapChipField::kBlockWidth / 2.0f;
				float z = 0;

				// Y座標：ブロック上面に合わせる
				float topY = mapField->GetBlockTopY(col, row);
				float y = topY + Enemy::kHeight / 2.0f + 0.1f; // 少し浮かせる

				positions.push_back({x, y, z});
			}
			++col;
		}
		++row;
	}

	return positions;
}

void EnemyManager::AddEnemy(Enemy* enemy) { enemies_.push_back(enemy); }

void EnemyManager::SpawnEnemy(const Vector3& pos) {
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_, camera_, pos);
	enemies_.push_back(newEnemy);
}

void EnemyManager::SetCameraManager(CameraManager* cameraManager) { cameraManager_ = cameraManager; }

void EnemyManager::SetScore(Score* score) { score_ = score; }
