#include "Enemy.h"
#include "../../DirectXGame/etc/MathUtilityFortext.h"
#include "../Player/Player.h"
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

	// ★ 敵タイプごとの見た目
	if (type_ == EnemyType::Jumper) {
		worldTransform_.scale_ = {0.65f, 0.65f, 0.65f}; // 少し大きい
	} else if (type_ == EnemyType::Dasher) {
		worldTransform_.scale_ = {0.55f, 0.55f, 0.55f}; // 細め
	} else {
		worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	}
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

	if (type_ == EnemyType::Dasher && isDashing_) {
		velocity_.x = std::clamp(velocity_.x, -kWalkSpeed * 2.0f, kWalkSpeed * 2.0f);
	}

	if (hitWall) {
		if (type_ == EnemyType::Dasher && isDashing_) {
			// 突進中に壁 → 突進中断
			isDashing_ = false;
			dashTimer_ = 0.0f;
			velocity_.x = direction_ * kWalkSpeed;
		}

		ReverseDirection();
	}
	// ===== 向き回転アニメーション =====
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

		float targetRotY = (direction_ > 0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;

		float t = std::clamp(turnTimer_ / kEnemyTurnTime, 0.0f, 1.0f);

		worldTransform_.rotation_.y = EaseInOut(targetRotY, turnFirstRotationY_, 1.0f - t);
	} else {
		// 通常はただの向きだけ反映
		worldTransform_.rotation_.y = (direction_ > 0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
	}

	// X方向は空中でも移動
	if (!(type_ == EnemyType::Dasher && isDashing_)) {
		worldTransform_.translation_.x += velocity_.x;
	}
	// ===== 4段階ジャンプ（Jumperのみ） =====
	if (type_ == EnemyType::Jumper) {

		jumpTimer_ += 1.0f / 120.0f;

		if (isOnGround_ && jumpTimer_ >= jumpInterval_) {
			float jumpHeights[4] = {0.35f, 0.5f, 0.65f, 0.8f};
			int idx = rand() % 4;
			velocity_.y = jumpHeights[idx];

			jumpInterval_ = RandRange(1.0f, 3.0f);
			jumpTimer_ = 0.0f;
		}

		if (type_ == EnemyType::Jumper && !isOnGround_) {
			worldTransform_.rotation_.z = std::sin(walkTimer_ * 10.0f) * 0.3f;
		}
	}

	// ===== 歩行モーション =====
	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.rotation_.x = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);

	worldTransform_.UpdateMatrix();

	//========================-突進========================
	if (type_ == EnemyType::Dasher && isOnGround_) {

		float dist = DistanceToPlayer();

		// プレイヤーが近い時だけ突進
		if (dist <= kDashTriggerDistance) {
			dashTimer_ += 1.0f / 60.0f;

			// 構え
			if (!isDashing_ && dashTimer_ < dashChargeTime_) {
				velocity_.x = 0.0f;

				// ★ ブルブル震える演出
				worldTransform_.rotation_.z = std::sin(dashTimer_ * 30.0f) * 0.15f;
			}

			// 突進開始
			if (!isDashing_ && dashTimer_ >= dashChargeTime_) {
				if (player_) {
					FacePlayer(player_);
				}

				isDashing_ = true;
				dashTimer_ = 0.0f;
				velocity_.x = direction_ * kWalkSpeed * 3.5f; // 超高速
			}
		} else {
			// プレイヤーが離れたらリセット
			dashTimer_ = 0.0f;
		}
	}
	if (type_ == EnemyType::Dasher && isDashing_) {

		dashTimer_ += 1.0f / 60.0f;

		if (dashTimer_ >= dashDuration_) {
			isDashing_ = false;
			dashTimer_ = 0.0f;
			velocity_.x = direction_ * kWalkSpeed; // 通常速度へ
		}
	}

	// ===== 弾発射（ランダム） =====
	if (type_ == EnemyType::Dasher || type_ == EnemyType::Jumper) {

		shootTimer_ += 1.0f / 60.0f;

		if (shootTimer_ >= shootInterval_) {

			// ランダム判定
			float r = RandRange(0.0f, 1.0f);
			if (r <= shootProbability_) {
				Shoot();
			}

			shootTimer_ = 0.0f;
		}
	}
}

void Enemy::Draw() {
	if (!model_)
		return;
	Vector3 pos = GetWorldPosition();
	char buf[128];
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

	// 例：死亡エフェクトや削除処理
	isDead_ = true;
}

void Enemy::FacePlayer(const Player* player) {
	if (!player)
		return;

	Vector3 p = player->GetWorldPosition();
	Vector3 e = GetWorldPosition();

	direction_ = (p.x > e.x) ? 1 : -1;

	worldTransform_.rotation_.y = (direction_ > 0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
}

float Enemy::DistanceToPlayer() {
	if (!player_)
		return FLT_MAX;

	float dx = player_->GetWorldPosition().x - GetWorldPosition().x;
	return std::abs(dx); // ★ 横距離だけ
}

void Enemy::Shoot() {
	if (!player_ || !enemyManager_)
		return;

	Vector3 enemyPos = GetWorldPosition();
	Vector3 playerPos = player_->GetWorldPosition();

	Vector3 dir = playerPos - enemyPos;
	dir.y = 0;

	float len = std::abs(dir.x);
	if (len <= 0.001f)
		return;

	dir.x /= len;

	Vector3 bulletVelocity = dir * 0.25f;

	enemyManager_->SpawnBullet(enemyPos + Vector3(direction_ * 0.6f, 0.3f, 0), bulletVelocity);

	bool toRight = (direction_ > 0);
	enemyManager_->SpawnBulletRandom(worldTransform_.translation_, toRight);
}
