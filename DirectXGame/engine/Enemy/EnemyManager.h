#pragma once
#include "../../engine/Player/Player.h"
#include "Enemy.h"
#include "engine/Enemy/EnemyParticle/EnemyDeathParticles.h"
#include "engine/Map/MapChipField.h"
#include <audio/Audio.h>
#include <vector>

class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize(KamataEngine::Model* enemyModel, Camera* camera, MapChipField* mapField, const std::string& csvPath);
	void Update(MapChipField* mapField);
	void Draw();
	void CheckAllCollisions(Player* player);
	void HandleEnemyCollisions();

	bool IsAllEnemyDefeated() const;

	std::vector<KamataEngine::Vector3> LoadEnemyPositionsFromCSV(const std::string& filename, MapChipField* mapField);

	void AddEnemy(Enemy* enemy);

	int UpdateScore();

private:
	std::vector<Enemy*> enemies_;
	KamataEngine::Model* enemyModel_;
	Camera* camera_;
	KamataEngine::Audio* audio_ = nullptr;

	std::vector<EnemyDeathParticles*> deathParticles_;
	Model* enemyDeathParticleModel_ = nullptr;

	uint32_t enemyDeathSE_ = 0;
};
