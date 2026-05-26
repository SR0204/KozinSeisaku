#pragma once
#include "../../Scene/SceneManager/SceneID.h"
#include "KamataEngine.h"
#include "PhaseState.h" // ★追加
#include <memory>
#include <optional>
#include <vector>

// 前方宣言
class Player;
class EnemyManager;
class Skydome;
class CameraManager;
class MapChipField;
class SceneManager;
class DeathParticles;
namespace KamataEngine {
class Model;
}

class PhaseManager {
public:
	PhaseManager() = default;
	~PhaseManager();

	void Initialize(
	    Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<KamataEngine::WorldTransform*>>* blocks, MapChipField* mapChipField,
	    SceneManager* sceneManager);

	// ★ switch文が無くなり、状態クラスに丸投げするだけのUpdate
	std::optional<SceneID> Update();
	void Draw();

	// ★ 状態を切り替えるための関数
	void ChangeState(std::unique_ptr<PhaseState> nextState);

	// ★ 状態クラス（Play/Death）からアクセスするためのゲッター・セッター
	Player* GetPlayer() const { return player_; }
	EnemyManager* GetEnemyManager() const { return enemyManager_; }
	Skydome* GetSkydome() const { return skydome_; }
	CameraManager* GetCameraManager() const { return cameraManager_; }
	std::vector<std::vector<KamataEngine::WorldTransform*>>* GetBlocks() const { return blocks_; }
	MapChipField* GetMapChipField() const { return mapChipField_; }

	DeathParticles* GetDeathParticles() const { return deathParticles_; }
	void SetDeathParticles(DeathParticles* dp) { deathParticles_ = dp; }

	KamataEngine::Model* GetDeathParticleModel() const { return deathParticleModel_; }
	void CreateDeathParticleModel();

private:
	// 共通保持ポインタ
	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	Skydome* skydome_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>>* blocks_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	// ★ enum phase_ を廃止し、現在の状態ポインタに変更
	std::unique_ptr<PhaseState> currentState_;

	// 死亡演出用
	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* deathParticleModel_ = nullptr;
};