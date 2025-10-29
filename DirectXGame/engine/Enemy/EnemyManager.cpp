#define NOMINMAX
#include "EnemyManager.h"
#include "../../DirectXGame/etc/MathUtilityForText.h" // IsColision 用
#include "../../engine/Player/Player.h"
#include <algorithm>
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

void EnemyManager::Initialize(KamataEngine::Model* enemyModel, Camera* camera) {
	enemyModel_ = enemyModel;
	camera_ = camera;

	audio_ = Audio::GetInstance();

	// === CSV から敵の配置をロード ===
	std::vector<Vector3> enemyPositions = LoadEnemyPositionsFromCSV("Resources/blocks.csv");

	// 敵の初期配置
	for (int i = 0; i < 4; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 pos = {10 + i * 5.0f, 5, 0};
		newEnemy->Initialize(enemyModel_, camera, pos);
		enemies_.push_back(newEnemy);
	}

	// デスパーティクル用モデル
	enemyDeathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

	// --- 効果音をロード ---
	// enemyDeathSE_ = Audio::GetInstance()->LoadWave("Resources/Sound/EnemySoundEffects/enemy_death.wav");
}

void EnemyManager::Update(MapChipField* mapField) {
	for (Enemy* enemy : enemies_) {
		enemy->Update(mapField); // MapChipField を渡して壁判定や重力処理を行う
	}

	for (auto it = deathParticles_.begin(); it != deathParticles_.end();) {
		(*it)->Update();
		if ((*it)->IsFinished()) {
			delete *it;
			it = deathParticles_.erase(it);
		} else {
			++it;
		}
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
			// 上から踏んだ判定
			if (player->GetVelocity().y < 0.0f && playerAABB.min.y > enemy->GetAABB().max.y - 5.0f) {
				// 踏んだ！即消し
				enemy->SetAlive(false);
				player->SetVelocityY(0.25f); // 軽くバウンド

				// --- 効果音を再生 ---
				// Audio::GetInstance()->PlayWave(enemyDeathSE_);

				// ここでデスパーティクルを生成
				EnemyDeathParticles* p = new EnemyDeathParticles();
				p->Initialize(enemyDeathParticleModel_, camera_, enemy->GetWorldPosition());
				deathParticles_.push_back(p);
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

std::vector<KamataEngine::Vector3> EnemyManager::LoadEnemyPositionsFromCSV(const std::string& filename) {
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
			if (std::stoi(cell) == 3) { // 3 の位置が敵
				positions.push_back({col * 5.0f, 5.0f, row * 5.0f});
				// X,Y,Z は適宜調整
			}
			++col;
		}
		++row;
	}
	return positions;
}

void EnemyManager::AddEnemy(Enemy* enemy) { enemies_.push_back(enemy); }
