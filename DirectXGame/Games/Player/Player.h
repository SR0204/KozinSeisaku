#pragma once
#include "../Collision/Collider.h"
#include "../LoadJsonFile/FileJson.h"
#include "../stage/StageID.h"
#include "KamataEngine.h"

class StageMapCollider;
struct PlayerData {
	std::vector<std::vector<StageType>> data;
	KamataEngine::Vector2 blockSize;
};

enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };
class Player {
public:
	void Initialize(std::vector<std::vector<StageType>> Data_, KamataEngine::Vector2 BlockSize_);
	void CheckExitPortalCollisionSphere();
	void Update();
	void Draw(const KamataEngine::Camera& camera);
	void DrawImGui();

	void Moves();

	void MoveInput();

	void MapCollision();

	void MapCollisionTop();
	void MapCollisionBottom();
	void MapCollisionLeft();
	void MapCollisionRight();

	void Move();

	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	KamataEngine::Vector3 GetCornerPos(Vector3 pos, Corner corner);

	bool GetPortal() { return isOnExitPortal_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Vector3 velocity_;
	KamataEngine::Vector3 acacceleration_;
	KamataEngine::Vector3 direction_;
	bool isJumping_ = false;            // ジャンプ中かどうか
	float targetAngleY_ = 90.0f;        // 初期は右向き
	int jumpCount_ = 0;                 // 現在のジャンプ回数
	const int maxJumpCount_ = 2;        // 最大ジャンプ数
	bool isHoldingJump_ = false;        // ジャンプ中にホールドしてるか
	float holdTimer_ = 0.0f;            // ジャンプホールド時間
	const float maxHoldTime_ = 0.2f;    // 最大ホールド時間（秒）
	const float holdJumpBoost_ = 0.01f; // フレームごとの追加ジャンプ力

	const float kFriction = 0.85f;

	// Json読み書き用のファイルアクセサ
	FileJson::FileAccessor* fileAccessor_;

	const std::string playerJson_ = "Player";

	PlayerData playerData_;

	bool isSave_ = false;

	StageMapCollider* stageMapCollider_ = nullptr;
	Input* input;
	const float kBlank = 1.0f;

private:
	float speed;
	float gravity;
	float jumpPower;
	float groundY;
	float rotationSpeed;

	// 追加: 球体コライダー用の半径
	float playerRadius_ = 0.5f; // プレイヤーの半径（例）
	bool isOnExitPortal_ = false;

public:
	bool IsOnExitPortal() const { return isOnExitPortal_; }
};