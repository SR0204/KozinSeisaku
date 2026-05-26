#include "PhaseManager.h"
#include "PhaseStateDeath.h"
#include "../../DirectXGame/engine/Player/Player.h"
#include "../../DirectXGame/engine/Map/Skydome/Skydome.h"
#include "../../DirectXGame/engine/Camera/CameraManager.h"
#include "../../DirectXGame/engine/Enemy/EnemyManager.h"
#include "../../DirectXGame/engine/Particle/DeathParticles.h"

std::optional<SceneID> PhaseStateDeath::Update(PhaseManager* context) {
	// ★元の case Phase::kDeath: の処理をそのまま移植
	context->GetSkydome()->Update();
	context->GetEnemyManager()->Update(context->GetMapChipField());
	context->GetCameraManager()->Update();

	// パーティクルの生成（初回のみ）
	if (!context->GetDeathParticles()) {
		// 必要なアセットモデルを作成
		context->CreateDeathParticleModel();

		DeathParticles* dp = new DeathParticles();
		dp->Initialize(context->GetDeathParticleModel(), context->GetCameraManager()->GetViewProjection(), context->GetPlayer()->GetWorldPosition());
		context->SetDeathParticles(dp);
	}

	context->GetDeathParticles()->Update();

	for (auto& line : *(context->GetBlocks())) {
		for (KamataEngine::WorldTransform* block : line) {
			if (block)
				block->UpdateMatrix();
		}
	}

	// パーティクル終了したら「GameOver」シーンを返す
	if (context->GetDeathParticles()->IsFinished()) {
		return SceneID::GameOver;
	}

	return std::nullopt;
}

void PhaseStateDeath::Draw(PhaseManager* context) {
	// ★元の Draw() にあった死亡パーティクルの描画処理を移植
	if (context->GetDeathParticles()) {
		context->GetDeathParticles()->Draw();
	}
}