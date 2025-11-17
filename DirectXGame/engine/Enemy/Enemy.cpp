#include "Enemy.h"
#include "../../DirectXGame/etc/MathUtilityFortext.h"
#include <algorithm>
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

	// ===== 重力 =====
	velocity_.y += kEnemyGravityAcceleration;
	if (velocity_.y < kEnemyLimitFallSpeed)
		velocity_.y = kEnemyLimitFallSpeed;

	// ===== 次の位置 =====
	Vector3 nextPos = worldTransform_.translation_;
	nextPos.x += velocity_.x;
	nextPos.y += velocity_.y;

	// ===== 床判定（左右2点でチェック） =====
	Vector3 bottomLeft = nextPos + Vector3(-kWidth / 2, -kHeight / 2 - 0.05f, 0);
	Vector3 bottomRight = nextPos + Vector3(kWidth / 2, -kHeight / 2 - 0.05f, 0);

	MapChipField::IndexSet idxL = mapField->GetMapChipIndexSetByPosition(bottomLeft);
	MapChipField::IndexSet idxR = mapField->GetMapChipIndexSetByPosition(bottomRight);

	float floorY = -999.0f;
	bool hitFloor = false;

	// 左側
	if (mapField->GetMapchipTypeByIndex(idxL.xIndex, idxL.yIndex) == MapChipType::kBlock) {
		floorY = mapField->GetRectByIndex(idxL.xIndex, idxL.yIndex).top;
		hitFloor = true;
	}

	// 右側
	if (mapField->GetMapchipTypeByIndex(idxR.xIndex, idxR.yIndex) == MapChipType::kBlock) {
		float rTop = mapField->GetRectByIndex(idxR.xIndex, idxR.yIndex).top;
		floorY = (std::max)(floorY, rTop); // 高い方の床
		hitFloor = true;
	}

	// ===== 床がある場合の処理 =====
	if (hitFloor && nextPos.y - kHeight / 2 <= floorY + 0.05f) {
		worldTransform_.translation_.y = floorY + kHeight / 2;
		if (velocity_.y < 0)
			velocity_.y = 0; // 落下中のみリセット
		isOnGround_ = true;
	} else {
		worldTransform_.translation_.y += velocity_.y;
		isOnGround_ = false;
	}

	// ===== 壁判定（X方向、地上のみ） =====
	bool hitWall = false;
	if (isOnGround_) {
		Vector3 checkSide = nextPos + Vector3((velocity_.x > 0 ? kWidth / 2 : -kWidth / 2), 0, 0);
		MapChipField::IndexSet idxSide = mapField->GetMapChipIndexSetByPosition(checkSide);
		if (mapField->GetMapchipTypeByIndex(idxSide.xIndex, idxSide.yIndex) == MapChipType::kBlock)
			hitWall = true;
	}

	if (hitWall)
		ReverseDirection();

	// X方向は空中でも移動
	worldTransform_.translation_.x += velocity_.x;

	// ===== 向き =====
	worldTransform_.rotation_.y = (velocity_.x > 0) ? std::numbers::pi_v<float> / 2 : -std::numbers::pi_v<float> / 2;

	// ===== 4段階ジャンプ =====
	jumpTimer_ += 1.0f / 120.0f;
	if (isOnGround_ && jumpTimer_ >= jumpInterval_) {
		// 4段階のジャンプ高さ（自然に見えるように小刻み）
		float jumpHeights[4] = {0.35f, 0.5f, 0.65f, 0.8f};
		int idx = rand() % 4; // ランダムに選択
		velocity_.y = jumpHeights[idx];

		jumpInterval_ = RandRange(1.0f, 3.0f);
		jumpTimer_ = 0.0f;
	}

	// ===== 歩行モーション =====
	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.rotation_.x = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
	if (!model_)
		return;
	Vector3 pos = GetWorldPosition();
	char buf[128];
	sprintf_s(buf, "Enemy::Draw called at (%.1f, %.1f, %.1f)\n", pos.x, pos.y, pos.z);
	OutputDebugStringA(buf);
	model_->Draw(worldTransform_, *camera_);
}

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

void Enemy::OnDead() {

	// スコア加算
	if (score_) {
		score_->AddScore(10);
	}

	// 例：死亡エフェクトや削除処理
	isDead_ = true;
}