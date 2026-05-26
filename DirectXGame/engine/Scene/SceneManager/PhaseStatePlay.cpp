#include "PhaseStatePlay.h"
#include "PhaseManager.h"
#include "PhaseStateDeath.h"
#include "../../DirectXGame/engine/Player/Player.h"
#include "../../DirectXGame/engine/Map/Skydome/Skydome.h"
#include "../../DirectXGame/engine/Camera/CameraManager.h"
#include "../../DirectXGame/engine/Enemy/EnemyManager.h"
#include "../../DirectXGame/engine/Particle/DeathParticles.h"

std::optional<SceneID> PhaseStatePlay::Update(PhaseManager* context) {
	// ★元の case Phase::kPlay: の処理をそのまま移植
	context->GetSkydome()->Update();
	context->GetPlayer()->Update();
	context->GetEnemyManager()->Update(context->GetMapChipField());
	context->GetCameraManager()->Update();

	for (auto& line : *(context->GetBlocks())) {
		for (KamataEngine::WorldTransform* block : line) {
			if (block)
				block->UpdateMatrix();
		}
	}

	context->GetEnemyManager()->CheckAllCollisions(context->GetPlayer());
	context->GetEnemyManager()->HandleEnemyCollisions();

	// プレイヤーの死亡チェック
	if (context->GetPlayer()->IsDead()) {
		// ★ここで次の「死亡状態」クラスへステート遷移させる！
		context->ChangeState(std::make_unique<PhaseStateDeath>());
	}

	return std::nullopt;
}

void PhaseStatePlay::Draw(PhaseManager* context) {
	// プレイ中は専用の追加描画なし
	(void)context;
}