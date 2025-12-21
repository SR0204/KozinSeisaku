#pragma once
#include "../../DirectXGame/etc/AABB.h"
#include "../../engine/Enemy/EnemyManager.h"
#include "../../engine/Score/Score.h"
#include "../Player/Player.h"
#include "engine/Map/MapChipField.h" // 追加
#include <3d/Model.h>
#include <3d/WorldTransform.h>

// 前方宣言
class Player;
class EnemyManager;

enum class EnemyState { Patrol, Chase };

enum class EnemyType {
	Normal, // ===============普通に踏める敵==============
	MidBoss // ===============中ボス====================
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

	/// <summary>
	/// 中ボス初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="camera"></param>
	/// <param name="pos"></param>
	void InitializeMidBoss(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& pos);

	/// <summary>
	/// 中ボス更新
	/// </summary>
	void UpdateMidBoss();

	/// <summary>
	/// 敵マネージャーのセッター
	/// </summary>
	/// <param name="manager"></param>
	void SetEnemyManager(EnemyManager* manager);

	EnemyType GetType() const { return type_; }

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

	EnemyState state_ = EnemyState::Patrol;
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

	//=====================中ボス用======================
	EnemyType type_ = EnemyType::Normal;
	float spawnTimer_ = 0; // スポーン時間
	const float spawnInterval_ = 2.5f;
	EnemyManager* enemyManager_ = nullptr;
};
