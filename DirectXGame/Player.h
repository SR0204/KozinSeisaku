#pragma once
#include "AABB.h"
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include<math/Vector3.h>

using namespace KamataEngine;

// 前方宣言
class MapChipField;
class Enemy;
/// <summary>
///	自キャラ
/// </summary>
class Player {

public: // 引数を書くところ
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, Camera* camera, const Vector3& position); // void Initialize(Model* model, ViewProjection* viewProjection);

	// 対応するGetterを作成
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 速度加算
	const Vector3& GetVelocity() const { return velocity_; }

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// キャラクターの当たり判定サイズ(0.0fとかにするとキャラクターが埋まったりする)
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	// 移動入力
	void InputMove();

	// 旋回制御
	void AnimateTurn();

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);

	// 接地状態の切り替え
	void cellingSwitch(const CollisionMapInfo& info);

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	// 角
	enum Corner {

		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void CheckMapCollisionHit(const CollisionMapInfo& info);

	void CellingContactHit(const CollisionMapInfo& info);

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// AABBを取得
	AABB GetAABB();

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

private: // 関数（メンバ変数）
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// 速度
	Vector3 velocity_ = {};

	// 角度補間

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 慣性移動
	static inline const float kAcceleration = 0.01f;

	// 速度減衰
	static inline const float kAttenuation = 0.01f;

	// 最大速度制限
	static inline const float kLimitRunSpeed = 0.1f;

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回時間＜秒＞
	static inline const float kTimeTurn = 0.3f;

	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.05f;

	// 最大落下速度（下方向）
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速（上方向）
	static inline const float kJumpAcceleration = 0.5f;

	static inline const float kBlank = 5;

	// 着地時の速度減衰率
	static inline const float kAttennuationLanding = 0.5f;

	static inline const float kAttennuationShift = 0.1f;

	// デスフラグ
	bool isDead_ = false;

	KamataEngine::Camera* camera_ = nullptr;
};