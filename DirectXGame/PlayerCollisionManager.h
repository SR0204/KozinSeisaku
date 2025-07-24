#pragma once
#include "MapChipField.h"
#include "Player.h"

class PlayerCollisionManager {
public:
	void Initialize(Player* player, MapChipField* mapField);

	void Update();         // プレイヤーの移動と当たり判定
	void ApplyCollision(); // 衝突結果の適用

	const AABB& GetAABB() const;

	enum Corner {
		kRightBottom = 0,
		kLeftBottom = 1,
		kRightTop = 2,
		kLeftTop = 3,
		kNumCorner = 4,
	};

	std::array<Vector3, kNumCorner> corners;

	// プレイヤーの当たり判定サイズと調整用定数
	static constexpr float kWidth = 1.0f;               // プレイヤーの幅（必要に応じて調整）
	static constexpr float kHeight = 2.0f;              // プレイヤーの高さ（必要に応じて調整）
	static constexpr float kBlank = 0.001f;             // めり込み防止の微小値
	static constexpr float kAttennuationShift = 0.05f;  // 接地チェックの下方向オフセット
	static constexpr float kAttennuationLanding = 0.2f; // 着地時の減速係数

	KamataEngine::WorldTransform worldTransform_;

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};

private:
	void CheckMapCollision();
	void CheckMapCollisionUp();
	void CheckMapCollisionDown();
	void CheckMapCollisionLeft();
	void CheckMapCollisionRight();
	void CellingContactHit();
	void CellingSwitch();

	Vector3 CornerPosition(const Vector3& center, Corner corner);



private:
	Player* player_ = nullptr;
	MapChipField* mapField_ = nullptr;
	CollisionMapInfo info_{};
	AABB aabb_;
};
