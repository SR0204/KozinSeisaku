#define NOMINMAX
#include "PlayerCollisionManager.h"
#include "math/Vector3.h"
#include <algorithm>
#include <array>
#include"MathUtilityForText.h"


void PlayerCollisionManager::Initialize(Player* player, MapChipField* mapField) {
	player_ = player;
	mapField_ = mapField;
}

void PlayerCollisionManager::Update() {
	// 移動前のAABB計算
	aabb_ = player_->GetAABB();

	info_ = {};
	info_.move = player_->GetVelocity(); // 毎フレーム移動予定

	CheckMapCollision();
	CellingSwitch();
	CellingContactHit();
}

void PlayerCollisionManager::ApplyCollision() { player_->worldTransform_.translation_ += info_.move; }

const AABB& PlayerCollisionManager::GetAABB() const { return aabb_; }

void PlayerCollisionManager::CheckMapCollision() {
	CheckMapCollisionUp();
	CheckMapCollisionDown();
	CheckMapCollisionLeft();
	CheckMapCollisionRight();
}

void PlayerCollisionManager::CheckMapCollisionUp() {
	if (info_.move.y <= 0)
		return;

	for (uint32_t i = 0; i < corners.size(); ++i) {
		corners[i] = CornerPosition(player_->worldTransform_.translation_ + info_.move, static_cast<Corner>(i));
	}

	bool hit = false;
	auto check = [&](Vector3 pos) {
		auto index = mapField_->GetMapChipIndexSetByPosition(pos);
		return mapField_->GetMapchipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock;
	};

	if (check(corners[Corner::kLeftTop]) || check(corners[Corner::kRightTop])) {
		hit = true;
	}

	if (hit) {
		auto index = mapField_->GetMapChipIndexSetByPosition(player_->worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		auto rect = mapField_->GetRectByIndex(index.xIndex, index.yIndex);
		info_.move.y = std::max(0.0f, rect.bottom - player_->worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info_.ceiling = true;
	}
}

void PlayerCollisionManager::CheckMapCollisionDown() {
	if (info_.move.y >= 0)
		return;

	for (uint32_t i = 0; i < corners.size(); ++i) {
		corners[i] = CornerPosition(player_->worldTransform_.translation_ + info_.move, static_cast<Corner>(i));
	}

	bool hit = false;
	auto check = [&](Vector3 pos) {
		auto index = mapField_->GetMapChipIndexSetByPosition(pos);
		return mapField_->GetMapchipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock;
	};

	if (check(corners[kLeftBottom]) || check(corners[kRightBottom])) {
		hit = true;
	}

	if (hit) {
		auto index = mapField_->GetMapChipIndexSetByPosition(player_->worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		auto rect = mapField_->GetRectByIndex(index.xIndex, index.yIndex);
		info_.move.y = std::max(0.0f, rect.top - player_->worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		info_.landing = true;
	}
}

void PlayerCollisionManager::CheckMapCollisionLeft() {
	if (info_.move.x >= 0)
		return;

	for (uint32_t i = 0; i < corners.size(); ++i) {
		corners[i] = CornerPosition(player_->worldTransform_.translation_ + info_.move, static_cast<Corner>(i));
	}

	bool hit = false;
	auto check = [&](Vector3 pos) {
		auto index = mapField_->GetMapChipIndexSetByPosition(pos);
		return mapField_->GetMapchipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock;
	};

	if (check(corners[kLeftTop]) || check(corners[kLeftBottom])) {
		hit = true;
	}

	if (hit) {
		auto index = mapField_->GetMapChipIndexSetByPosition(player_->worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));
		auto rect = mapField_->GetRectByIndex(index.xIndex, index.yIndex);
		info_.move.x = std::max(0.0f, rect.left - player_->worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info_.ceiling = true;
	}
}

void PlayerCollisionManager::CheckMapCollisionRight() {
	if (info_.move.x <= 0)
		return;

	for (uint32_t i = 0; i < corners.size(); ++i) {
		corners[i] = CornerPosition(player_->worldTransform_.translation_ + info_.move, static_cast<Corner>(i));
	}

	bool hit = false;
	auto check = [&](Vector3 pos) {
		auto index = mapField_->GetMapChipIndexSetByPosition(pos);
		return mapField_->GetMapchipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock;
	};

	if (check(corners[kRightTop]) || check(corners[kRightBottom])) {
		hit = true;
	}

	if (hit) {
		auto index = mapField_->GetMapChipIndexSetByPosition(player_->worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		auto rect = mapField_->GetRectByIndex(index.xIndex, index.yIndex);
		info_.move.x = std::max(0.0f, rect.right - player_->worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info_.ceiling = true;
	}
}

void PlayerCollisionManager::CellingContactHit() {
	if (info_.ceiling) {
		player_->SetVelocityY(0);
	}
}

void PlayerCollisionManager::CellingSwitch() {
	if (player_->IsOnGround()) {
		if (player_->GetVelocity().y > 0.0f) {
			player_->SetOnGround(false);
		} else {
			// 接地判定
			std::array<Vector3, kNumCorner> corners;
			for (uint32_t i = 0; i < corners.size(); ++i) {
				corners[i] = CornerPosition(player_->worldTransform_.translation_ + info_.move, static_cast<Corner>(i));
			}

			auto check = [&](Vector3 pos) {
				auto index = mapField_->GetMapChipIndexSetByPosition(pos + Vector3(0, -kAttennuationShift, 0));
				return mapField_->GetMapchipTypeByIndex(index.xIndex, index.yIndex) == MapChipType::kBlock;
			};

			if (!check(corners[kLeftBottom]) && !check(corners[kRightBottom])) {
				player_->SetOnGround(false);
			}
		}
	} else {
		if (info_.landing) {
			player_->SetOnGround(true);
			player_->SetVelocityY(0.0f);
			player_->AttenuateX(kAttennuationLanding);
		}
	}
}

Vector3 PlayerCollisionManager::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0},
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}
