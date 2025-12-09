#pragma once
#include "../../DirectXGame/etc/AABB.h"
#include "../../engine/Particle/ParticleSystem.h"
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <math/Vector3.h>

using namespace KamataEngine;

// 前方宣言
class MapChipField;
class Enemy;

/// <summary>
/// 自キャラクラス
/// </summary>
class Player {

public:
	Player();
	~Player();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="model">使用するモデル</param>
	/// <param name="camera">使用するカメラ</param>
	/// <param name="position">初期座標</param>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// ワールド変換のGetter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 現在の速度を取得
	const Vector3& GetVelocity() const { return velocity_; }

	// 左右の向き
	enum class LRDirection {
		kRight, // 右向き
		kLeft,  // 左向き
	};

	/// <summary>
	/// マップチップフィールドの登録
	/// </summary>
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// プレイヤーの当たり判定サイズ
	static inline const float kWidth = 0.5f;
	static inline const float kHeight = 0.5f;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 移動入力処理
	/// </summary>
	void InputMove();

	/// <summary>
	/// 旋回演出制御
	/// </summary>
	void AnimateTurn();

	// マップ衝突情報
	struct CollisionMapInfo {
		bool ceiling = false; // 天井と接触したか
		bool landing = false; // 地面に着地したか
		bool hitWall = false; // 壁と衝突したか
		Vector3 move;         // 衝突判定結果による補正移動量
	};

	/// <summary>
	/// マップとプレイヤーの総合的な当たり判定
	/// </summary>
	void CheckMapCollision(CollisionMapInfo& info);

	/// <summary>
	/// // 上方向衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionUp(CollisionMapInfo& info);

	/// <summary>
	/// 下方向衝突判定（着地判定）
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionDown(CollisionMapInfo& info);

	/// <summary>
	/// 左方向衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	/// <summary>
	/// 右方向衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionRight(CollisionMapInfo& info);

	/// <summary>
	/// 衝突後の処理（速度調整など）
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionHit(const CollisionMapInfo& info);

	/// <summary>
	/// 天井接触時の処理
	/// </summary>
	/// <param name="info"></param>
	void CellingContactHit(const CollisionMapInfo& info);

	/// <summary>
	/// 接地状態の切り替え（着地/空中）
	/// </summary>
	/// <param name="info"></param>
	void cellingSwitch(const CollisionMapInfo& info);

	/// <summary>
	/// 敵との衝突時の処理
	/// </summary>
	/// <param name="info"></param>
	void OnCollision(const Enemy* enemy);

	/// <summary>
	/// プレイヤー四隅の座標取得用
	/// </summary>
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		kNumCorner
	};

	/// <summary>
	/// 指定した角のワールド座標を返す
	/// </summary>
	/// <param name="center"></param>
	/// <param name="corner"></param>
	/// <returns></returns>
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	/// <summary>
	/// プレイヤーのワールド座標を返す
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition();

	/// <summary>
	/// AABBを返す
	/// </summary>
	/// <returns></returns>
	AABB GetAABB();

	/// <summary>
	/// デスフラグ
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// y方向速度のSetter（ジャンプなど）
	/// </summary>
	/// <param name="vy"></param>
	void SetVelocityY(float vy) { velocity_.y = vy; }

	/// <summary>
	/// 敵を踏んだ時のバウンド力
	/// </summary>
	float GetBouncePower() const { return bouncePower_; }

	// 連続踏みつけボーナス
	int consecutiveBouncePoints_ = 5;
	static const int kMaxBouncePoints = 5;

	// ボーナス表示スプライト
	KamataEngine::Sprite* bouncePointSprites_[kMaxBouncePoints] = {};

	// 着地したらボーナスリセット
	void OnLand() { consecutiveBouncePoints_ = 1; }

private:
	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// ワールド変換
	WorldTransform worldTransform_;

	// 使用モデル
	Model* model_ = nullptr;

	// 移動速度
	Vector3 velocity_ = {};

	// ジャンプ中かどうか
	bool isJump_ = false;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回演出のタイマー
	float turnTimer_ = 0.0f;

	// 接地しているか
	bool onGround_ = true;

	// 加速度（毎フレーム加算）
	static inline const float kAcceleration = 0.01f;

	// 減速係数
	static inline const float kAttenuation = 0.01f;

	// 最大走行速度
	static inline const float kLimitRunSpeed = 0.1f;

	// 左右向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回にかかる時間(秒)
	static inline const float kTimeTurn = 0.3f;

	// 重力加速度
	static inline const float kGravityAcceleration = 0.05f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.5f;

	static inline const float kBlank = 0.01f;

	// 着地時の速度減衰
	static inline const float kAttennuationLanding = 0.5f;

	// シフト入力時の減衰
	static inline const float kAttennuationShift = 0.1f;

	// デスフラグ
	bool isDead_ = false;

	// 使用カメラ
	Camera* camera_ = nullptr;

	// ヒップドロップ中か
	bool isHipDrop_ = false;

	// 空中でヒップドロップ可能か
	bool hipDropReady_ = false;

	// HP
	int Hp_ = 5;

	// --- 無敵関連 ---
	bool isInvincible_ = false;    // 無敵状態か
	int invincibleTimer_ = 0;      // 残り無敵フレーム
	int invincibleDuration_ = 180; // 無敵時間（3秒）

	// --- パーティクル ---
	ParticleSystem* dashParticles_ = nullptr;    // ダッシュ時パーティクル
	ParticleSystem* hipDropParticles_ = nullptr; // ヒップドロップ時パーティクル
	Model* quadModel_ = nullptr;                 // パーティクル描画用の四角形モデル
	Model* CreateQuadModel();                    // 四角形モデル生成

	// 敵を踏んだときのバウンド力
	float bouncePower_ = 0.35f;
};
