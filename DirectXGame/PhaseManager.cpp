#include "PhaseManager.h"
using namespace KamataEngine;

void PhaseManager::Initialize(Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<WorldTransform*>>* blocks) {
	player_ = player;
	enemyManager_ = enemyManager;
	skydome_ = skydome;
	cameraManager_ = cameraManager;
	blocks_ = blocks;
}

void PhaseManager::Update() {
	switch (phase_) {
	case Phase::kPlay:
		skydome_->Update();
		player_->Update();
		enemyManager_->Update();
		cameraManager_->Update();

		for (auto& line : *blocks_) {
			for (WorldTransform* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}

		isDead_ = player_->IsDead();
		if (isDead_) {
			phase_ = Phase::kDeath;
			deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);
		}

		enemyManager_->CheckAllCollisions(player_);
		break;

	case Phase::kDeath:
		skydome_->Update();
		enemyManager_->Update();
		cameraManager_->Update();

		for (auto& line : *blocks_) {
			for (WorldTransform* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}
		break;
	}
}
