#pragma once
#include "Enemy.h"
#include <3d/Model.h>
#include "Player.h"
#include <vector>
#include<KamataEngine.h>

class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize(KamataEngine::Model* enemyModel, Camera* camera);
	void Update();
	void Draw();
	void CheckAllCollisions(Player* player);

private:
	std::vector<Enemy*> enemies_;
	KamataEngine::Model* enemyModel_;
	Camera* camera_;
};
