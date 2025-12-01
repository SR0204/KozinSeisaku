#pragma once
#include "../../engine/Camera/CameraManager.h"
#include "../../engine/Player/Player.h"
#include "../../engine/Score/Score.h"
#include "Enemy.h"
#include "engine/Enemy/EnemyParticle/EnemyDeathParticles.h"
#include "engine/Map/MapChipField.h"
#include <audio/Audio.h>
#include <vector>

/// <summary>
/// Enemy.cppをまとめて当たり判定とかをまとめるクラス
/// </summary>
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

	void SetScore(Score* score) { score_ = score; }

	void SpawnEnemy(const Vector3& pos);

	void SetCameraManager(CameraManager* cameraManager);

private:
	std::vector<Enemy*> enemies_;
	KamataEngine::Model* enemyModel_;
	Camera* camera_;
	KamataEngine::Audio* audio_ = nullptr;

	std::vector<EnemyDeathParticles*> deathParticles_;
	Model* enemyDeathParticleModel_ = nullptr;

	uint32_t enemyDeathSE_ = 0;

	Score* score_ = nullptr;

	//------------スポーン---------------//
	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 3.0f;       // 3秒ごとにスポーン
	std::vector<Vector3> spawnPoints_; // CSVの3の位置

	//-------------リアクション----------------//
	float hitStopTime_ = 0.0f;
	const float kHitStopDuration_ = 0.1f; // 0.06秒ほど止める

	CameraManager* cameraManager_;
};
