#include "Enemy.h"
#include "../../DirectXGame/etc/MathUtilityFortext.h"
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
	direction_ = 1;

	walkTimer_ = 0.0f;

	worldTransform_.scale_ = {0.5, 0.5, 0.5};
}

void Enemy::Update(MapChipField* mapField) {
	if (collisionCooldown_ > 0)
		collisionCooldown_--;

	// ===== X方向の壁判定 =====
	Vector3 nextPos = worldTransform_.translation_ + Vector3(velocity_.x, 0, 0);
	Vector3 checkPos = nextPos;
	checkPos.x += (velocity_.x > 0) ? kWidth / 2.0f : -kWidth / 2.0f;
	MapChipField::IndexSet idx = mapField->GetMapChipIndexSetByPosition(checkPos);
	if (mapField->GetMapchipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kBlock) {
		ReverseDirection();
	} else {
		worldTransform_.translation_.x = nextPos.x;
	}

	// ===== 向き =====
	if (velocity_.x > 0) {
		worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
	} else if (velocity_.x < 0) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	}

	// ===== 重力 =====
	velocity_.y += kEnemyGravityAcceleration;
	if (velocity_.y < kEnemyLimitFallSpeed) {
		velocity_.y = kEnemyLimitFallSpeed;
	}

	// ===== 地面チェック =====
	Vector3 footPos = worldTransform_.translation_;
	footPos.y -= kHeight / 2.0f;
	MapChipField::IndexSet footIdx = mapField->GetMapChipIndexSetByPosition(footPos);

	isOnGround_ = false;
	if (mapField->GetMapchipTypeByIndex(footIdx.xIndex, footIdx.yIndex) == MapChipType::kBlock) {
		MapChipField::Rect blockRect = mapField->GetRectByIndex(footIdx.xIndex, footIdx.yIndex);
		worldTransform_.translation_.y = blockRect.top + kHeight / 2.0f;
		velocity_.y = 0;
		isOnGround_ = true;
	} else {
		worldTransform_.translation_.y += velocity_.y;
	}

	// ===== 不規則ジャンプ処理 =====
	jumpTimer_ += 1.0f / 60.0f;

	if (isOnGround_ && jumpTimer_ >= jumpInterval_) {
		// ランダムジャンプ力（0.3〜0.8）
		float jumpPower = RandRange(0.3f, 0.8f);
		velocity_.y = jumpPower;

		// 次のジャンプまでの時間をランダムに設定（1〜3秒）
		jumpInterval_ = RandRange(1.0f, 3.0f);

		jumpTimer_ = 0.0f;
	}

	// ===== 歩行モーション =====
	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.rotation_.x = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

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
