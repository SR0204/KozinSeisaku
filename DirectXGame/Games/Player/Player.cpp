#include "Player.h"
#include "../Collision/StageMapCollider.h"
#include "input/Input.h"
#include <DirectXMath.h>
#include <algorithm>

using namespace KamataEngine;
using namespace DirectX;
using namespace MathUtility;

void Player::Initialize(std::vector<std::vector<StageType>> Data_, Vector2 BlockSize_) {

	// FileAccessorの初期化
	fileAccessor_ = nullptr;
	// JSONファイル名を指定してFileAccessorを初期化 (相対パスを使用)
	fileAccessor_ = new FileJson::FileAccessor("Resources/Json/Player.json");

	playerData_.data = Data_;
	playerData_.blockSize = BlockSize_;

	stageMapCollider_ = new StageMapCollider;
	stageMapCollider_->SetBlockSize(playerData_.blockSize);
	stageMapCollider_->SetData(playerData_.data);

	velocity_ = {0.0f, 0.0f, 0.0f};

	model_ = Model::CreateFromOBJ("player");
	worldTransform_.Initialize();
	// worldTransform_.translation_ = {0.0f, -5.0f, -30.0f};
	// worldTransform_.translation_ = fileAccessor_->ReadVector3(playerJson_, "1stPos", Vector3());
	for (uint32_t y = 0; y < playerData_.data.size(); y++) {
		playerData_.data[y].resize(playerData_.data[y].size());
		for (uint32_t x = 0; x < playerData_.data[y].size(); x++) {
			if (playerData_.data[y][x] == StageType::kFirstPlayer || playerData_.data[y][x] == StageType::kEntrancePortal) {
				Vector3 playerPos = {1.0f * x * playerData_.blockSize.x, 1.0f * (playerData_.data.size() - 1 - y) * playerData_.blockSize.x, 0};
				worldTransform_.translation_ = playerPos;
			}
		}
	}

	// Y軸を +90度回転して右向きにする
	worldTransform_.rotation_.y = XMConvertToRadians(90.0f);

	speed = fileAccessor_->Read(playerJson_, "speed", float());                 // キャラのスピード
	gravity = fileAccessor_->Read(playerJson_, "gravity", float());             // 重力加速度
	jumpPower = fileAccessor_->Read(playerJson_, "jumpPower", float());         // ジャンプの初速度
	groundY = fileAccessor_->Read(playerJson_, "groundY", float());             // 地面Y座標
	rotationSpeed = fileAccessor_->Read(playerJson_, "rotationSpeed", float()); // 補間係数（滑らかさ）
}

void Player::Update() {

	input = Input::GetInstance();

	Moves();

	// 現在の角度（度）
	float currentAngle = XMConvertToDegrees(worldTransform_.rotation_.y);

	// 差分（度）
	float delta = targetAngleY_ - currentAngle;

	// 最初の方向転換かどうかを判定するフラグ
	bool firstTurnDone_ = false;

	// 正規化（-180° ～ 180°に収める）
	if (delta > 180.0f) {
		delta -= 360.0f;
	} else if (delta < -180.0f) {
		delta += 360.0f;
	}

	// 左(-90) → 右(90) は必ず反時計回り
	if (currentAngle > -135.0f && currentAngle < -45.0f && targetAngleY_ == 90.0f) {
		delta = -270.0f;
	}

	// 最初の右(90) → 左(-90) だけは時計回り
	if (!firstTurnDone_ && currentAngle > 45.0f && currentAngle < 135.0f && targetAngleY_ == -90.0f) {
		delta = +270.0f;       // 時計回り
		firstTurnDone_ = true; // 以降は通常処理
	}

	// 新しい角度を計算
	float newAngle = currentAngle + delta * rotationSpeed;

	// ラジアンにして反映
	worldTransform_.rotation_.y = XMConvertToRadians(newAngle);

	//// ジャンプ入力（スペースキー）
	// if (input->TriggerKey(DIK_SPACE) && jumpCount_ < maxJumpCount_) {
	//	// velocityY_ = jumpPower;
	//	isJumping_ = true;
	//	jumpCount_++; // ジャンプ回数を加算
	// }

	//// 重力・落下処理
	// if (isJumping_) {
	//	//velocityY_ -= gravity;
	//	//worldTransform_.translation_.y += velocityY_;

	//	//// 着地判定
	//	//if (worldTransform_.translation_.y <= groundY) {
	//	//	worldTransform_.translation_.y = groundY;
	//	//	isJumping_ = false;
	//	//	//velocityY_ = 0.0f;
	//	//	jumpCount_ = 0; // 着地したらジャンプ回数リセット
	//	//}
	//}

	worldTransform_.UpdateMatrix();
	CheckExitPortalCollisionSphere(); // 追加
}

void Player::Draw(const Camera& camera) {

	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}

void Player::DrawImGui() {
#ifdef DEBUG_
	ImGui::Begin("Player");
	ImGui::DragFloat3("transform", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
#endif // DEBUG_
}

void Player::Moves() {
	MoveInput();
	MapCollision();
	Move();
}

void Player::MoveInput() {
	bool isMove = false;
	bool isFrictionX = false;
	bool isFrictionY = false;

	if (input->PushKey(DIK_A)) {
		direction_.x -= 1.0f;
		targetAngleY_ = -90.0f; // 左向き
		isMove = true;
	}
	if (input->PushKey(DIK_D)) {
		direction_.x += 1.0f;
		targetAngleY_ = 90.0f; // 右向き
		isMove = true;
	}

	// ジャンプ入力（スペースキー）
	if (input->TriggerKey(DIK_SPACE) && jumpCount_ < maxJumpCount_) {
		direction_.y += 1.0f;
		isMove = true;
		jumpCount_++; // ジャンプ回数を加算
	}

	direction_ = Normalize(direction_);

	acacceleration_ = direction_;
	acacceleration_ *= speed;
	velocity_ += acacceleration_;
	velocity_.y += gravity;

	if (!isMove) {
		velocity_ *= kFriction;
	} else {
		if ((velocity_.x > 0.0f && direction_.x < 0.0f) || (velocity_.x < 0.0f && direction_.x > 0.0f) || direction_.x == 0.0f) {
			isFrictionX = true;
		}
		if ((velocity_.y > 0.0f && direction_.y < 0.0f) || (velocity_.y < 0.0f && direction_.y > 0.0f) || direction_.y == 0.0f) {
			isFrictionY = true;
		}

		if (isFrictionX) {
			velocity_.x *= kFriction;
		}
		if (isFrictionY) {
			velocity_.y *= kFriction;
		}
	}
	float speedRete = 1.0f;
	speedRete *= speed;
	velocity_.x = std::clamp<float>(velocity_.x, -speedRete, speedRete);
	velocity_.y = std::clamp<float>(velocity_.y, -speedRete, speedRete);
}

void Player::MapCollision() {
	MapCollisionTop();
	MapCollisionBottom();
	MapCollisionLeft();
	MapCollisionRight();
}

void Player::MapCollisionTop() {
	if (velocity_.y >= 0.0f) {
		return;
	}
	Vector3 newPos = worldTransform_.translation_;
	newPos.y += velocity_.y;
	Vector3 topLeft = GetCornerPos(newPos, Corner::kLeftTop);
	Vector3 topRight = GetCornerPos(newPos, Corner::kRightTop);
	bool hit = false;

	if (StageType::kBlock == stageMapCollider_->GetMapChipType(topLeft)) {
		hit = true;
	}
	if (StageType::kBlock == stageMapCollider_->GetMapChipType(topRight)) {
		hit = true;
	}

	if (hit) {
		StageMapCollider::Rect mapChipRect = stageMapCollider_->GetRectByIndex(static_cast<int>(topLeft.x), static_cast<int>(topLeft.y));
		velocity_.y = std::min<float>(0.0f, mapChipRect.bottom - worldTransform_.translation_.y + playerData_.blockSize.y / 2 + kBlank);
	}
}

void Player::MapCollisionBottom() {
	if (velocity_.y <= 0.0f) {
		return;
	}
	Vector3 newPos = worldTransform_.translation_;
	newPos.y += velocity_.y;
	Vector3 bottomLeft = GetCornerPos(newPos, Corner::kLeftBottom);
	Vector3 bottomRight = GetCornerPos(newPos, Corner::kRightBottom);
	bool hit = false;

	if (StageType::kBlock == stageMapCollider_->GetMapChipType(bottomLeft)) {
		hit = true;
	}
	if (StageType::kBlock == stageMapCollider_->GetMapChipType(bottomRight)) {
		hit = true;
	}

	if (hit) {
		StageMapCollider::Rect mapChipRect = stageMapCollider_->GetRectByIndex(static_cast<int>(bottomLeft.x), static_cast<int>(bottomLeft.y));
		velocity_.y = std::max<float>(0.0f, mapChipRect.top - worldTransform_.translation_.y - playerData_.blockSize.y / 2 - kBlank);
	}
}

void Player::MapCollisionLeft() {
	if (velocity_.x >= 0.0f) {
		return;
	}
	Vector3 newPos = worldTransform_.translation_;
	newPos += velocity_;
	Vector3 topLeft = GetCornerPos(newPos, Corner::kLeftTop);
	Vector3 bottomLeft = GetCornerPos(newPos, Corner::kLeftBottom);
	bool hit = false;

	if (StageType::kBlock == stageMapCollider_->GetMapChipType(topLeft)) {
		hit = true;
	}
	if (StageType::kBlock == stageMapCollider_->GetMapChipType(bottomLeft)) {
		hit = true;
	}

	if (hit) {
		StageMapCollider::Rect mapChipRect = stageMapCollider_->GetRectByIndex(static_cast<int>(topLeft.x), static_cast<int>(topLeft.y));
		velocity_.x = std::min<float>(0.0f, mapChipRect.right - worldTransform_.translation_.x + playerData_.blockSize.x / 2 + kBlank);
	}
}

void Player::MapCollisionRight() {
	if (velocity_.x <= 0.0f) {
		return;
	}
	Vector3 newPos = worldTransform_.translation_;
	newPos += velocity_;
	Vector3 topRight = GetCornerPos(newPos, Corner::kRightTop);
	Vector3 bottomRight = GetCornerPos(newPos, Corner::kRightBottom);
	bool hit = false;

	if (StageType::kBlock == stageMapCollider_->GetMapChipType(topRight)) {
		hit = true;
	}
	if (StageType::kBlock == stageMapCollider_->GetMapChipType(bottomRight)) {
		hit = true;
	}

	if (hit) {
		StageMapCollider::Rect mapChipRect = stageMapCollider_->GetRectByIndex(static_cast<int>(topRight.x), static_cast<int>(topRight.y));
		velocity_.x = std::max<float>(0.0f, mapChipRect.left - worldTransform_.translation_.x - playerData_.blockSize.x / 2 - kBlank);
	}
}

void Player::Move() { worldTransform_.translation_ += velocity_; }

KamataEngine::Vector3 Player::GetCornerPos(Vector3 pos, Corner corner) {
	std::unordered_map<Corner, Vector3> cornerOffsets = {
	    {Corner::kLeftTop,     {playerData_.blockSize.x / -2, playerData_.blockSize.y / -2, 0.0f}},
	    {Corner::kRightTop,    {playerData_.blockSize.x / 2, playerData_.blockSize.y / -2, 0.0f} },
	    {Corner::kLeftBottom,  {playerData_.blockSize.x / -2, playerData_.blockSize.y / 2, 0.0f} },
	    {Corner::kRightBottom, {playerData_.blockSize.x / 2, playerData_.blockSize.y / 2, 0.0f}  }
    };
	Vector3 newPos = pos;
	newPos += cornerOffsets[corner];
	return newPos;
}

// 追加: 球体同士の当たり判定関数
bool CheckSphereCollision(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB) {
	float dx = centerA.x - centerB.x;
	float dy = centerA.y - centerB.y;
	float dz = centerA.z - centerB.z;
	float distSq = dx * dx + dy * dy + dz * dz;
	float radiusSum = radiusA + radiusB;
	return distSq <= (radiusSum * radiusSum);
}

void Player::CheckExitPortalCollisionSphere() {
	isOnExitPortal_ = false;
	// マップデータから出口ポータルを探す
	for (uint32_t y = 0; y < playerData_.data.size(); ++y) {
		for (uint32_t x = 0; x < playerData_.data[y].size(); ++x) {
			if (playerData_.data[y][x] == StageType::kExitPortal) {
				// 出口ポータルの中心座標
				Vector3 portalCenter = stageMapCollider_->GetMapChipPositionByIndex(x, y);
				float portalRadius = std::min<float>(playerData_.blockSize.x, playerData_.blockSize.y) / 2.0f; // ポータルの半径（例）
				// プレイヤーの中心座標
				Vector3 playerCenter = worldTransform_.translation_;

				// 球体同士の当たり判定
				if (CheckSphereCollision(playerCenter, playerRadius_, portalCenter, portalRadius)) {
					isOnExitPortal_ = true;
					return;
				}
			}
		}
	}
}
