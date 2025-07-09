#pragma once
#include "AABB.h"
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include "Player.h"

// 前方宣言
class Player;

// 敵
class Enemy {

public: // 引数を書くところ
	/// <summary>
	/// 初期化
	/// </summary>
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	// 衝突応答
	void OnCollision(const Player* player);

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// キャラクターの当たり判定サイズ(0.0fとかにするとキャラクターが埋まったりする)
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	AABB GetAABB();

private:
	// 渡して3D描画
	//  ワールド変換データ
	WorldTransform worldTransform_;

	// 敵モデル
	Model* model_ = nullptr;

	// ビュープロジェクションのポインタ
	Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	// 速度
	Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.01f;

	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 0.1f;

	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 3.0f;

	// 経過時間
	float walkTimer_ = 0.0f;
};