#pragma once
#include "AABB.h"
#include <3d/Camera.h>
#include <3d/Model.h>
#include "PlayerCollisionManager.h"
#include <3d/WorldTransform.h>
#include <memory>
#include "math/Vector3.h"

class Player {
public:
	Player();
	~Player();

	void Initialize(Model* model, Camera* camera, MapChipField* mapField, const Vector3& position);
	void Update();
	void Draw();

	const AABB& GetAABB() const;
	Vector3 GetVelocity() const { return velocity_; }
	void SetVelocityY(float y) { velocity_.y = y; }
	void AttenuateX(float factor) { velocity_.x *= factor; }

	bool IsOnGround() const { return isOnGround_; }
	void SetOnGround(bool flag) { isOnGround_ = flag; }

	WorldTransform worldTransform_; // ★衝突マネージャが使うためpublicでもOK

	inline Vector3 Normalize(const Vector3& v) {
		float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len == 0.0f)
			return {0.0f, 0.0f, 0.0f};
		return {v.x / len, v.y / len, v.z / len};
	}

private:
	void InputMove();
	void AnimateTurn();

private:
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	std::unique_ptr<PlayerCollisionManager> collisionManager_;

	Vector3 velocity_{};
	float moveSpeed_ = 0.3f;
	float gravity_ = -0.2f;
	bool isOnGround_ = false;

	static constexpr float kWidth = 1.0f;
	static constexpr float kHeight = 2.0f;
};
