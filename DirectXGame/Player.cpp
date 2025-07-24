#include "Player.h"
#include "MathUtilityForText.h"
#include "math/Vector3.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <input/Input.h>


using namespace KamataEngine;

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, MapChipField* mapField, const Vector3& position) {
	assert(model);
	assert(camera);
	assert(mapField);

	model_ = model;
	camera_ = camera;

	worldTransform_.translation_ = position;
	worldTransform_.Initialize();

	// ★ プレイヤー当たり判定マネージャ初期化
	collisionManager_ = std::make_unique<PlayerCollisionManager>();
	collisionManager_->Initialize(this, mapField);
}

void Player::Update() {
	InputMove();

	// 重力加算
	velocity_.y += gravity_;

	// ★ 当たり判定計算＆結果適用
	collisionManager_->Update();
	collisionManager_->ApplyCollision();

	// モデル行列更新
	worldTransform_.UpdateMatrix();

	// アニメーションや回転処理
	AnimateTurn();
}

void Player::InputMove() {
	Input* input = Input::GetInstance();

	velocity_.x = 0.0f;
	velocity_.z = 0.0f;

	if (input->PushKey(DIK_W)) {
		velocity_.z -= moveSpeed_;
	}
	if (input->PushKey(DIK_S)) {
		velocity_.z += moveSpeed_;
	}
	if (input->PushKey(DIK_A)) {
		velocity_.x -= moveSpeed_;
	}
	if (input->PushKey(DIK_D)) {
		velocity_.x += moveSpeed_;
	}
}

void Player::AnimateTurn() {
	if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
		Vector3 dir = Normalize(Vector3{velocity_.x, 0.0f, velocity_.z});
		worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
	}
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

const AABB& Player::GetAABB() const {
	return AABB{
	    .min = worldTransform_.translation_ + Vector3(kWidth / 2.0f, kHeight / 2.0f, 0),
	    .max = worldTransform_.translation_ + Vector3(kWidth / 2.0f, kHeight / 2.0f, 0),
	};
}
