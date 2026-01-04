#include "EnemyBullet.h"
#include "../../../etc/MathUtilityForText.h"
#include <cassert>

void EnemyBullet::Initialize(Model* model, Camera* camera, const Vector3& pos, const Vector3& velocity) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	velocity_ = velocity;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f}; // 弾サイズ

	lifeTimer_ = 0.0f;
	isDead_ = false;

	worldTransform_.UpdateMatrix();
}

void EnemyBullet::Update() {
	worldTransform_.translation_ += velocity_;

	lifeTimer_ += 1.0f / 60.0f;
	if (lifeTimer_ >= kLifeTime_) {
		isDead_ = true;
	}

	worldTransform_.UpdateMatrix();
}

void EnemyBullet::Draw() {
	if (!model_)
		return;

	model_->Draw(worldTransform_, *camera_);
}

Vector3 EnemyBullet::GetWorldPosition() const { return {worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]}; }

AABB EnemyBullet::GetAABB() {
	Vector3 pos = GetWorldPosition();
	return {
	    {pos.x - 0.2f, pos.y - 0.2f, pos.z - 0.2f},
        {pos.x + 0.2f, pos.y + 0.2f, pos.z + 0.2f}
    };
}

void EnemyBullet::Kill() { isDead_ = true; }
