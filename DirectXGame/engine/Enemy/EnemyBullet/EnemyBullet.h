#pragma once
#include "../../DirectXGame/etc/AABB.h"
#include "KamataEngine.h"
#include "engine/Map/MapChipField.h"
#include <3d/Model.h>
#include <3d/WorldTransform.h>

class EnemyBullet {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& pos, const Vector3& velocity);
	void Update();
	void Draw();

	AABB GetAABB();
	Vector3 GetWorldPosition() const;

	void Kill();
	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	Vector3 velocity_;
	bool isDead_ = false;

	float lifeTimer_ = 0.0f;
	static constexpr float kLifeTime_ = 3.0f; // 3秒
};