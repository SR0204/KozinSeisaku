// ParticleSystem.h
#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/Vector3.h"
#include <vector>

namespace KamataEngine {

struct Particle {
	Vector3 position;
	Vector3 velocity;
	float lifetime;
};

/// <summary>
/// 主にプレイヤーのパーティクルエフェクトを作るクラス
/// </summary>

class ParticleSystem {
public:
	ParticleSystem(KamataEngine::Camera* camera);

	void Initialize(int maxParticles);
	/// <summary>
	/// パーティクル放出関係
	/// </summary>
	/// <param name="position"></param>
	/// <param name="velocity"></param>
	/// <param name="lifetime"></param>
	void Emit(const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity, float lifetime);
	void Update();
	void Draw();

private:
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Model* quadModel_ = nullptr; // 内部で生成
	KamataEngine::WorldTransform worldTransform_;
	std::vector<Particle> particles_;
	int maxParticles_ = 0;
};

} // namespace KamataEngine
