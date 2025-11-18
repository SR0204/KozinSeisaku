#include "SpawnManager.h"

void SpawnManager::Initialize(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

void SpawnManager::Update(float deltaTime) {
	spawnTimer_ += deltaTime;

	if (spawnTimer_ >= spawnInterval_) {
		spawnTimer_ = 0;

		// ★敵をスポーン（自由に位置変更OK）
		Vector3 pos = {10.0f, 0.0f, 10.0f};

		enemyManager_->SpawnEnemy(pos);
	}
}
