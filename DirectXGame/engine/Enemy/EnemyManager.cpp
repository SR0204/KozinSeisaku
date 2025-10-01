#define NOMINMAX
#include "EnemyManager.h"
#include "../../DirectXGame/etc/MathUtilityForText.h" // IsColision 用
#include "../../engine/Player/Player.h"
#include <algorithm>

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

	// 敵の初期配置
	for (int i = 0; i < 4; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 pos = {10 + i * 5.0f, 5, 0};
		newEnemy->Initialize(enemyModel_, camera, pos);
		enemies_.push_back(newEnemy);
	}
}

void EnemyManager::Update(MapChipField* mapField) {
	for (Enemy* enemy : enemies_) {
		enemy->Update(mapField); // MapChipField を渡して壁判定や重力処理を行う
	}
}

void EnemyManager::Draw() {
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
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
