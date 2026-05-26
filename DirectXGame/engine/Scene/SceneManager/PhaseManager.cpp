#include "PhaseManager.h"
#include "../../DirectXGame/engine/Camera/CameraManager.h"
#include "../../DirectXGame/engine/Enemy/EnemyManager.h"
#include "../../DirectXGame/engine/Map/SkyDome/SkyDome.h"
#include "../../DirectXGame/engine/Particle/DeathParticles.h"
#include "../DirectXGame/engine/Player/Player.h"
#include "3d/Model.h"
#include "PhaseStatePlay.h" // ★初期状態のためにインクルード

using namespace KamataEngine;

PhaseManager::~PhaseManager() {
	delete deathParticles_;
	delete deathParticleModel_;
}

void PhaseManager::Initialize(
    Player* player, EnemyManager* enemyManager, Skydome* skydome, CameraManager* cameraManager, std::vector<std::vector<WorldTransform*>>* blocks, MapChipField* mapChipField,
    SceneManager* sceneManager) {

	player_ = player;
	enemyManager_ = enemyManager;
	skydome_ = skydome;
	cameraManager_ = cameraManager;
	blocks_ = blocks;
	mapChipField_ = mapChipField;
	sceneManager_ = sceneManager;

	// ★ 初期状態を「PlayState」クラスのインスタンスにする！
	ChangeState(std::make_unique<PhaseStatePlay>());
}

std::optional<SceneID> PhaseManager::Update() {
	// ★ switch文を完全撤廃。現在の状態に丸投げするだけ！
	if (currentState_) {
		return currentState_->Update(this);
	}
	return std::nullopt;
}

void PhaseManager::Draw() {
	// ★ if文によるフェーズチェックを撤廃。現在の状態に丸投げ！
	if (currentState_) {
		currentState_->Draw(this);
	}
}

// ★ 状態遷移を行う関数
void PhaseManager::ChangeState(std::unique_ptr<PhaseState> nextState) { currentState_ = std::move(nextState); }

// 死亡パーティクル用モデル生成
void PhaseManager::CreateDeathParticleModel() {
	if (!deathParticleModel_) {
		deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);
	}
}