#pragma once
#include "../../engine/Enemy/Enemy.h"
#include "../../engine/Enemy/EnemyManager.h"
#include <vector>
/// <summary>
/// 敵を定期的に湧くようにしてスコアを競えるようにするクラス
/// </summary>
class SpawnManager {
public:
	void Initialize(EnemyManager* enemyManager);
	void Update(float deltaTime);

private:
	EnemyManager* enemyManager_;
	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 2.0f; // 2秒に1体スポーン
};
