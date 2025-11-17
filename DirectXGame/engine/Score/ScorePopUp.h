#pragma once
#include "KamataEngine.h"
#include <math/Vector3.h>

class ScorePopUp {
public:
	void Initialize(const KamataEngine::Vector3& worldPos, int score);
	void Update();
	void Draw(const KamataEngine::Camera& camera);

	bool IsFinished() const { return timer_ >= lifeTime_; }

private:
	KamataEngine::Vector3 position_;
	int score_;
	float timer_ = 0.0f;
	float lifeTime_ = 60.0f; // 1秒くらい
	float alpha_ = 1.0f;
};