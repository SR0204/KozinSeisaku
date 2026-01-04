#pragma once
#include "../../DirectXGame/etc/AABB.h"
#include "../../engine/Enemy/EnemyManager.h"
#include "../../engine/Score/Score.h"
#include "engine/Map/MapChipField.h" // 追加
#include <3d/Model.h>
#include <3d/WorldTransform.h>

// 前方宣言
class Player;
class EnemyManager;

enum class EnemyState { Ground, Jump, Dash };

enum class EnemyType {
	Normal, // 通常
	Jumper, // ジャンプ状態
	Dasher, // 突進状態
	Aim,    // プレイヤーを見る
};

/// <summary>
/// 敵のベースを作るクラス
/// </summary>
class Enemy {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新処理（MapChipFieldを使う）
	/// </summary>
	void Update(MapChipField* mapField);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突応答
	/// </summary>
	/// <param name="player"></param>
	void OnCollision(const Player* player);

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.7f;
	static inline const float kHeight = 0.7f;

	AABB GetAABB();

	/// <summary>
	/// 生存確認
	/// </summary>
	/// <param name="alive"></param>
	void SetAlive(bool alive) { isAlive_ = alive; }
	bool IsAlive() const { return isAlive_; }

	/// <summary>
	/// 方向反転（velocity_.x も更新）
	/// </summary>
	void ReverseDirection() {
		direction_ *= -1;
		velocity_.x = kWalkSpeed * direction_;
		// ---- 回転アニメ開始 ----
		turnFirstRotationY_ = worldTransform_.rotation_.y;
		turnTimer_ = kEnemyTurnTime;
	}

	/// <summary>
	/// 向き
	/// </summary>
	/// <returns></returns>
	int GetDirection() const { return direction_; }

	/// <summary>
	/// 位置
	/// </summary>
	/// <param name="x"></param>
	void SetWorldX(float x) { worldTransform_.translation_.x = x; }

	void AddWorldX(float dx) { worldTransform_.translation_.x += dx; }

	int collisionCooldown_ = 0; // フレーム単位

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }
	void OnDead();

	void SetType(EnemyType type) { type_ = type; }
	EnemyType GetType() const { return type_; }

	/// <summary>
	/// プレイヤーを確認する
	/// </summary>
	/// <param name="player"></param>
	void FacePlayer(const Player* player);

	void SetPlayer(Player* player) { player_ = player; }

	bool IsDashing() const { return isDashing_; }

	float DistanceToPlayer();

	/// <summary>
	/// 射撃用
	/// </summary>
	void Shoot();

	void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// 敵モデル
	Model* model_ = nullptr;

	// ビュープロジェクションのポインタ
	Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.03f;

	// 速度
	Vector3 velocity_ = {};

	// 歩行アニメーション関連
	static inline const float kWalkMotionTime = 3.0f;
	float walkTimer_ = 0.0f;

	// 重力関連
	static inline const float kEnemyGravityAcceleration = -0.05f;
	static inline const float kEnemyLimitFallSpeed = -0.5f;

	bool isAlive_ = true;

	int direction_ = 1; // 右向き = 1, 左向き = -1

	float jumpTimer_ = 0.0f;    // ジャンプ間隔を計るタイマー
	float jumpInterval_ = 2.0f; // 2秒ごとにジャンプ
	bool isOnGround_ = false;   // 地面にいるかどうかのフラグ

	// ==============================================
	// min～max の範囲でランダムな float を返す関数
	// ==============================================
	float RandRange(float min, float max) { return min + (max - min) * (rand() / static_cast<float>(RAND_MAX)); }

	EnemyState state_ = EnemyState::Ground;
	float detectionRange_ = 5.0f; // プレイヤー検知距離

	// デスフラグ
	bool isDead_ = false;

	// ===== 回転アニメ用 =====
	float turnTimer_ = 0.0f;
	float turnFirstRotationY_ = 0.0f;
	static inline constexpr float kEnemyTurnTime = 0.25f; // 回転にかかる時間

	float EaseInOut(float end, float start, float t) {
		float tt = t * t * (3 - 2 * t); // 3t² - 2t³
		return start + (end - start) * tt;
	}

	EnemyType type_ = EnemyType::Normal;

	// 突進用
	float dashTimer_ = 0.0f;
	float dashChargeTime_ = 0.6f; // 構え時間
	float dashDuration_ = 0.4f;   // 突進時間
	bool isDashing_ = false;
	static constexpr float kDashTriggerDistance = 10.0f; // 突進開始距離（調整用）

	//==========プレイヤー============
	Player* player_ = nullptr;

	//=================--射撃用====================
	float shootTimer_ = 0.0f;
	float shootInterval_ = 2.0f;    // 最低発射間隔
	float shootProbability_ = 0.3f; // 30%で撃つ

	EnemyManager* enemyManager_ = nullptr;
};
