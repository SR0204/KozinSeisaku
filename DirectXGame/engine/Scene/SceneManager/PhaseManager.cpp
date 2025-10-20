#include "PhaseManager.h"
#include "../../Scene/SceneManager/SceneID.h"
#include "../../Scene/SceneManager/SceneManager.h"

using namespace KamataEngine;

void PhaseManager::Initialize(
    Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<WorldTransform*>>* blocks, MapChipField* mapChipField,
    SceneManager* sceneManager) {

	player_ = player;
	enemyManager_ = enemyManager;
	skydome_ = skydome;
	cameraManager_ = cameraManager;
	blocks_ = blocks;
	mapChipField_ = mapChipField;
	sceneManager_ = sceneManager; // ★ シーンマネージャを保持
	phase_ = Phase::kPlay;
}

// PhaseManager.cpp
std::optional<SceneID> PhaseManager::Update() {
	switch (phase_) {
	case Phase::kTitle:
		break;

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

		enemyManager_->CheckAllCollisions(player_);
		enemyManager_->HandleEnemyCollisions();

		isDead_ = player_->IsDead();
		if (isDead_) {
			phase_ = Phase::kDeath;
			if (!deathParticleModel_) {
				deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);
			}
			deathPosition_ = player_->GetWorldPosition();
		}
		break;

	case Phase::kDeath:
		skydome_->Update();
		enemyManager_->Update(mapChipField_);
		cameraManager_->Update();

		if (!deathParticles_) {
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(deathParticleModel_, cameraManager_->GetViewProjection(), deathPosition_);
		}

		deathParticles_->Update();

		for (auto& line : *blocks_) {
			for (WorldTransform* block : line) {
				if (block)
					block->UpdateMatrix();
			}
		}

		// パーティクル終了したら「GameOver」シーンを返す
		if (deathParticles_->IsFinished()) {
			return SceneID::GameOver;
		}
		break;
	}

	return std::nullopt; // シーン切り替えなし
}


void PhaseManager::Draw() {
	if (phase_ == Phase::kDeath && deathParticles_) {
		deathParticles_->Draw();
	}
}
