#include "PhaseManager.h"
using namespace KamataEngine;

void PhaseManager::Initialize(
    Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<WorldTransform*>>* blocks, MapChipField* mapChipField) {
	player_ = player;
	enemyManager_ = enemyManager;
	skydome_ = skydome;
	cameraManager_ = cameraManager;
	blocks_ = blocks;
	mapChipField_ = mapChipField;
}

void PhaseManager::Update() {
	switch (phase_) {
	case Phase::kTitle:

	case Phase::kPlay:
		skydome_->Update();
		player_->Update();
		enemyManager_->Update(mapChipField_);
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
		enemyManager_->HandleEnemyCollisions();
		break;

	case Phase::kDeath:
		skydome_->Update();
		enemyManager_->Update(mapChipField_);
		cameraManager_->Update();

		if (!deathParticles_) {
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(deathParticleModel_, cameraManager_->GetViewProjection(), player_->GetWorldPosition());
		}
		deathParticles_->Update();

		for (auto& line : *blocks_) {
			for (WorldTransform* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}
		break;
	}
}

void PhaseManager::Draw() {
	if (phase_ == Phase::kDeath && deathParticles_) {
		deathParticles_->Draw();
	}
}