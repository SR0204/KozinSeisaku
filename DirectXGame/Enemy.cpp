#include "Enemy.h"
#include "MathUtilityFortext.h"
#include <cassert>
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	model_ = model;
	camera_ = camera;

	// 初期回転（左向き）
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	velocity_ = {kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
}

void Enemy::Update(MapChipField* mapField) {

	if (collisionCooldown_ > 0)
		collisionCooldown_--;

	// ===== X方向の壁判定 =====
	Vector3 nextPos = worldTransform_.translation_ + Vector3(velocity_.x, 0, 0);

	// 進行方向の先端
	Vector3 checkPos = nextPos;
	checkPos.x += (velocity_.x > 0) ? kWidth / 2.0f : -kWidth / 2.0f;

	MapChipField::IndexSet idx = mapField->GetMapChipIndexSetByPosition(checkPos);
	if (mapField->GetMapchipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kBlock) {
		// 壁 → 方向転換
		velocity_.x *= -1;
	} else {
		// 移動
		worldTransform_.translation_.x = nextPos.x;
	}

	// ===== 左右の向き判定 =====
	// 初期が左向きなので、右移動時は -90°, 左移動時は +90°
	if (velocity_.x > 0) {
		worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
	} else if (velocity_.x < 0) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	}

	// ===== 重力適用 =====
	velocity_.y += kEnemyGravityAcceleration;
	if (velocity_.y < kEnemyLimitFallSpeed) {
		velocity_.y = kEnemyLimitFallSpeed;
	}

	// 足元座標
	Vector3 footPos = worldTransform_.translation_;
	footPos.y -= kHeight / 2.0f;
	MapChipField::IndexSet footIdx = mapField->GetMapChipIndexSetByPosition(footPos);

	if (mapField->GetMapchipTypeByIndex(footIdx.xIndex, footIdx.yIndex) == MapChipType::kBlock) {
		// 地面あり → 高さをそろえて落下停止
		MapChipField::Rect blockRect = mapField->GetRectByIndex(footIdx.xIndex, footIdx.yIndex);
		worldTransform_.translation_.y = blockRect.top + kHeight / 2.0f;
		velocity_.y = 0;
	} else {
		// 落下
		worldTransform_.translation_.y += velocity_.y;
	}

	// ===== アニメーション（歩行モーション） =====
	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.rotation_.x = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	// ===== 行列更新 =====
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

void Enemy::OnCollision(const Player* player) { (void)player; }

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}
