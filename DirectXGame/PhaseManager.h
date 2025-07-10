#pragma once
#include "CameraManager.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Skydome.h"
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>

class PhaseManager {
public:
	enum class Phase { kPlay, kDeath };

	void Initialize(Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<WorldTransform*>>* blocks);

	void Update();

	Phase GetPhase() const { return phase_; }

	bool IsDead() const { return isDead_; }

	Model* GetDeathParticleModel() const { return deathParticleModel_; }

private:
	Phase phase_ = Phase::kPlay;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	Skydome* skydome_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	std::vector<std::vector<WorldTransform*>>* blocks_ = nullptr;

	bool isDead_ = false;
	Model* deathParticleModel_ = nullptr;
};
