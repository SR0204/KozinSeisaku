#pragma once
#include "../../DirectXGame/engine/Camera/CameraManager.h"
#include "../../DirectXGame/engine/Enemy/EnemyManager.h"
#include "../../DirectXGame/engine/Player/Player.h"
#include "engine/Map/MapChipField.h"
#include "engine/Map/SkyDome/SkyDome.h"
#include "engine/Particle/DeathParticles.h"
#include "engine/Scene/SceneManager/SceneManager.h"
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>

class PhaseManager {
public:
	enum class Phase { kTitle, kPlay, kDeath };

	void Initialize(
	    Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<WorldTransform*>>* blocks, MapChipField* mapChipField,
	    SceneManager* sceneManager);

	void Update();

	void Draw();

	Phase GetPhase() const { return phase_; }

	bool IsDead() const { return isDead_; }

	Model* GetDeathParticleModel() const { return deathParticleModel_; }

private:
	Phase phase_ = Phase::kTitle;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	Skydome* skydome_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	std::vector<std::vector<WorldTransform*>>* blocks_ = nullptr;

	bool isDead_ = false;
	Model* deathParticleModel_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	SceneManager* sceneManager_ = nullptr;
};
