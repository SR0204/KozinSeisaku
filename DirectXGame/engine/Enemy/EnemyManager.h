#pragma once
#include "../../engine/Camera/CameraManager.h"
#include "../../engine/Player/Player.h"
#include "../../engine/Score/Score.h"
#include "../Enemy/EnemyBullet/EnemyBullet.h"
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

	/// <summary>
	/// プレイヤーと敵の当たり判定
	/// </summary>
	/// <param name="player"></param>
	void CheckAllCollisions(Player* player);

	/// <summary>
	/// AABBの当たり判定
	/// </summary>
	void HandleEnemyCollisions();

	bool IsAllEnemyDefeated() const;

	/// <summary>
	/// csvファイル読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <param name="mapField"></param>
	/// <returns></returns>
	std::vector<KamataEngine::Vector3> LoadEnemyPositionsFromCSV(const std::string& filename, MapChipField* mapField);

	/// <summary>
	/// 追加の敵の関数
	/// </summary>
	/// <param name="enemy"></param>
	void AddEnemy(Enemy* enemy);

	/// <summary>
	/// スポーン
	/// </summary>
	/// <param name="pos"></param>
	void SpawnEnemy(const Vector3& pos);

	/// <summary>
	/// カメラ
	/// </summary>
	/// <param name="cameraManager"></param>
	void SetCameraManager(CameraManager* cameraManager);

	/// <summary>
	/// スコア管理
	/// </summary>
	/// <param name="score"></param>
	void SetScore(Score* score);

	/// <summary>
	/// 倒した時のスコア表示
	/// </summary>
	/// <param name="score"></param>
	void SetStompScore(int score) { stompScore_ = score; }

	/// <summary>
	/// プレイヤーのセッター
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 弾生成
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="vel"></param>
	void SpawnBullet(const Vector3& pos, const Vector3& vel);

	/// <summary>
	/// 弾をランダムで飛ばすようにする
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="toRight"></param>
	void SpawnBulletRandom(const Vector3& pos, bool toRight);

private:
	std::vector<Enemy*> enemies_;
	KamataEngine::Model* enemyModel_;
	Camera* camera_;
	KamataEngine::Audio* audio_ = nullptr;

	std::vector<EnemyDeathParticles*> deathParticles_;
	Model* enemyDeathParticleModel_ = nullptr;

	uint32_t enemyDeathSE_ = 0;

	Player* player_ = nullptr;

	//------------スポーン---------------//
	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 3.0f;       // 3秒ごとにスポーン
	std::vector<Vector3> spawnPoints_; // CSVの3の位置

	//-------------リアクション----------------//
	float hitStopTime_ = 0.0f;
	const float kHitStopDuration_ = 0.1f; // 0.06秒ほど止める

	CameraManager* cameraManager_;

	//--------------スコア---------------------//
	Score* score_ = nullptr;

	int stompScore_ = 5; // デフォルト値

	//=========================弾管理===============
	std::vector<std::unique_ptr<EnemyBullet>> bullets_;
	Model* bulletModel_ = nullptr;
};
