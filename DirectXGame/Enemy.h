#pragma once
#include "AABB.h"
#include "MapChipField.h" // 追加
#include "Player.h"
#include <3d/Model.h>
#include <3d/WorldTransform.h>

// 前方宣言
class Player;

// 敵
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

	// 衝突応答
	void OnCollision(const Player* player);

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	AABB GetAABB();

	void SetAlive(bool alive) { isAlive_ = alive; }
	bool IsAlive() const { return isAlive_; }

	// 方向反転（velocity_.x も更新）
	void ReverseDirection() {
		direction_ *= -1;
		velocity_.x = kWalkSpeed * direction_;
	}

	int GetDirection() const { return direction_; }

	void SetWorldX(float x) { worldTransform_.translation_.x = x; }

	void AddWorldX(float dx) { worldTransform_.translation_.x += dx; }

	int collisionCooldown_ = 0; // フレーム単位

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
};
