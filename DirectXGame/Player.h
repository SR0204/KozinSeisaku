#pragma once

#include <3d/Camera.h>
#include <3d/DebugCamera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include "KamataEngine.h"
#include "MathUtilityFortext.h"

class MapChipField;

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
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, Vector3 position); // void Initialize(Model* model, ViewProjection* viewProjection);

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	void InputMove();

	void AnimateTurn();

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool Ceiling = false; // 天井衝突フラグ
		bool landing = false; // 着地フラグ
		bool HitWall = false; // 壁接触フラグ
		Vector3 move;         // 移動量
	};

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	static inline const float kBlank = 1.0f;

	// 判定結果を反映して移動させる
	void CheckMapCollisionHit(const CollisionMapInfo& info);

	// 天井に接触している場合の処理
	void CeilingContact(const CollisionMapInfo& info);

	// 接地状態の切り替え
	void GroundedCondition(const CollisionMapInfo& info);

private: // 関数（メンバ変数）
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// 速度
	Vector3 velocity_;

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

	KamataEngine::Camera* camera_ = nullptr;

	// マップチップのフィールド
	MapChipField* mapChipField_ = nullptr;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.5f;
	static inline const float kAttenuationShift = 0.1f;
};