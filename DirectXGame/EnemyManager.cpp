#include "EnemyManager.h"
#include "MathUtilityForText.h"

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

	// 敵の初期配置など
	for (int i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 pos = {10 + i * 5.0f, 5, 0};
		newEnemy->Initialize(enemyModel_, camera, pos);
		enemies_.push_back(newEnemy);
	}
}

void EnemyManager::Update() {
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
}

void EnemyManager::Draw() {
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
}

void EnemyManager::CheckAllCollisions(Player* player) {
	AABB playerAABB = player->GetAABB();
	for (Enemy* enemy : enemies_) {
		if (IsColision(playerAABB, enemy->GetAABB())) {
			player->OnCollision(enemy);
			enemy->OnCollision(player);
		}
	}
}
