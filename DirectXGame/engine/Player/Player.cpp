#define NOMINMAX
#include "Player.h"
#include "../../DirectXGame/etc/MathUtilityFortext.h"
#include "../../engine/Map/MapManager.h"
#include <2d/DebugText.h>
#include <algorithm>
#include <cassert>
#include <input/Input.h>
#include <numbers>

using namespace KamataEngine;

Player::Player() {
	for (auto& s : bouncePointSprites_) {
		s = nullptr;
	}
}

Player::~Player() {
	delete dashParticles_;
	delete hipDropParticles_;
	delete quadModel_;
}

/// <summary>
/// 初期化
/// </summary>
void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// Nullポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position; // 初期配置
	worldTransform_.scale_ = {0.5, 0.5, 0.5};

	// 初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	dashParticles_ = new ParticleSystem(camera_);
	dashParticles_->Initialize(50);

	hipDropParticles_ = new ParticleSystem(camera_);
	hipDropParticles_->Initialize(50);
}

/// <summary>
/// 更新処理
/// </summary>
void Player::Update() {
	InputMove();

	// 衝突チェック情報を作成
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	// マップとの衝突判定
	CheckMapCollision(collisionMapInfo);

	// 衝突結果を反映（位置を動かす）
	CheckMapCollisionHit(collisionMapInfo);

	// 天井ヒットなど
	CellingContactHit(collisionMapInfo);
	cellingSwitch(collisionMapInfo);

	// --- パーティクル発生 ---
	dashParticles_->Update();
	hipDropParticles_->Update();

	// 回転制御
	AnimateTurn();

	// --- 無敵タイマー処理 ---
	if (isInvincible_) {
		invincibleTimer_--;
		if (invincibleTimer_ <= 0) {
			isInvincible_ = false;
			invincibleTimer_ = 0;
		}
	}

	// 行列更新
	worldTransform_.UpdateMatrix();
}

/// <summary>
/// 描画処理
/// </summary>
void Player::Draw() {

	// 無敵中は点滅（4フレームに1回非表示）
	if (isInvincible_ && ((invincibleTimer_ / 4) % 2 == 0)) {
		return; // 描画スキップ（チカチカする）
	}

	dashParticles_->Draw();
	hipDropParticles_->Draw();

	model_->Draw(worldTransform_, *camera_);
}

void Player::InputMove() {
	auto* input = Input::GetInstance();

	// 通常移動・ダッシュ移動
	const float walkSpeed = 0.2f;    // 通常速度
	const float dashSpeed = 0.4f;    // ダッシュ速度（Shift押下時）
	const float gravity = 0.03f;     // 重力加速度
	const float jumpPower = 0.6f;    // ジャンプ力
	const float maxFallSpeed = 1.0f; // 落下速度上限

	// Shiftキー押下でダッシュ
	bool isDash = input->PushKey(DIK_LSHIFT) || input->PushKey(DIK_RSHIFT);
	float moveSpeed = isDash ? dashSpeed : walkSpeed;

	// 横移動
	velocity_.x = 0.0f;

	if (input->PushKey(DIK_D)) {
		velocity_.x = moveSpeed;

		if (lrDirection_ != LRDirection::kRight) {
			// ターン開始
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn; // 0.2f など
		}

	} else if (input->PushKey(DIK_A)) {
		velocity_.x = -moveSpeed;

		if (lrDirection_ != LRDirection::kLeft) {
			// ターン開始
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
	}

	// ジャンプ（地上でのみ）
	if (onGround_ && (input->TriggerKey(DIK_W) || input->TriggerKey(DIK_SPACE))) {
		velocity_.y = jumpPower;
		onGround_ = false;
	}

	// ヒップドロップ発動
	if (!onGround_ && !isHipDrop_) {
		if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
			isHipDrop_ = true;
			velocity_.y = std::min(velocity_.y, -0.2f); // 落下方向に少し勢いをつける程度
		}
	}

	// 重力
	if (!onGround_) {
		float currentGravity = gravity;

		// ヒップドロップ中は重力を強くする
		if (isHipDrop_) {
			currentGravity = 0.1f; // 普段の約3倍の重力で加速していく
		}

		velocity_.y -= currentGravity;
		if (velocity_.y < -maxFallSpeed) {
			velocity_.y = -maxFallSpeed;
		}
	}
}

void Player::AnimateTurn() {

	if (turnTimer_ > 0.0f) {

		turnTimer_ -= 1.0f / 60.0f;
		float t = 1.0f - (turnTimer_ / kTimeTurn); // 0→1

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,        // 右向き
		    std::numbers::pi_v<float> * 3.0f / 2.0f, // 左向き
		};

		float destination = destinationRotationYTable[(uint32_t)lrDirection_];

		// スムーズに向きを補間
		worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destination, t);
	}
}

// 呼び出し
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	// 上昇がない場合は処理しない
	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	// 各コーナーの位置を取得（移動後の位置）
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックに当たった場合
	if (hit) {
		// めり込み補正
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, kHeight / 2.0f, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));

		// 天井ヒットフラグ
		info.ceiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	// 下降あり？
	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	// MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲短形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.landing = true;
	}
}

// 右の当たり判定
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	// 右あり？
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	// MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲短形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.ceiling = true;
	}
}

// 左の当たり判定
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	// 左あり？
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	//  MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲短形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.ceiling = true;
	}
}

void Player::cellingSwitch(const CollisionMapInfo& info) {

	if (onGround_) {
		// 接地状態の処理
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定

			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kAttennuationShift, 0));
			mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			// MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kAttennuationShift, 0));
			mapChipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下なら空中状態に切り替え
			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 空中状態の処理
		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える(落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttennuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;

			// ヒップドロップ中なら解除
			if (isHipDrop_) {
				isHipDrop_ = false;
				// （任意）着地時にパーティクルを出すなど
				// CreateHipDropEffect(); ← あればここで呼ぶ
			}
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {

	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0},
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::CellingContactHit(const CollisionMapInfo& info) {

	if (info.ceiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;

		// 天井に当たったブロックのインデックスを取得
		Vector3 headPos = worldTransform_.translation_ + Vector3(0, kHeight / 2.0f, 0);
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(headPos);
	}
}

Vector3 Player::GetWorldPosition() const {

	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnDamage() {

	// 無敵中ならダメージを受けない
	if (isInvincible_)
		return;

	// HP 減少
	Hp_--;

	// 無敵時間開始
	isInvincible_ = true;
	invincibleTimer_ = invincibleDuration_;

	// ノックバック（例）
	velocity_.y = 0.2f;
	velocity_.x = (velocity_.x >= 0) ? -0.2f : 0.2f;

	// SE 再生（あれば）
	// Audio::GetInstance()->PlayWave(damageSE_);

	// 死亡判定
	if (Hp_ <= 0) {
		isDead_ = true;
	}
}

// Quadモデルを作成する
KamataEngine::Model* Player::CreateQuadModel() {

	// Particle.obj をロード
	quadModel_->CreateFromOBJ("deathParticle", true);

	// 必要ならスケールや初期化処理
	quadModel_->StaticInitialize(); // もし Initialize が必要なら

	return quadModel_;
}

void Player::CheckMapCollisionHit(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.move;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;

	if (isDead_ || isInvincible_)
		return; // 既に死亡 or 無敵中なら何もしない

	Hp_--;

	// ダメージリアクション
	velocity_.y = 0.3f;
	worldTransform_.translation_.x += 0.3f;

	// 無敵状態に入る
	isInvincible_ = true;
	invincibleTimer_ = invincibleDuration_;

	if (Hp_ <= 0) {
		isDead_ = true;
	}
}