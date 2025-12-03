#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/ObjectColor.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "etc/MathUtilityForText.h"
#include <vector>

/// <summary>
/// 敵を倒した時のエフェクトを追加するクラス
/// </summary>

class EnemyDeathParticles {
public:
	static constexpr int kNumParticles = 6;
	static constexpr float kDuration = 1.0f; // 秒

	~EnemyDeathParticles();

	EnemyDeathParticles() = default;

	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	/// <summary>
	/// 終了判定
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return isFinished_; }

private:
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	std::vector<WorldTransform*> worldTransforms_;
	ObjectColor objectColor_;
	Vector4 color_{1, 1, 1, 1};
	float counter_ = 0.0f;
	bool isFinished_ = false;

	//----------パーティクルの大きさ-------------//
	float initialScale_ = 0.5f;
};
