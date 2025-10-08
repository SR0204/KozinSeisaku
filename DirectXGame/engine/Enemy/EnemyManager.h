#pragma once
#include "../../engine/Player/Player.h"
#include "Enemy.h"
#include "engine/Map/MapChipField.h"
#include <vector>

class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize(KamataEngine::Model* enemyModel, Camera* camera);
	void Update(MapChipField* mapField);
	void Draw();
	void CheckAllCollisions(Player* player);
	void HandleEnemyCollisions();

	bool IsAllEnemyDefeated() const;

private:
	std::vector<Enemy*> enemies_;
	KamataEngine::Model* enemyModel_;
	Camera* camera_;
};
