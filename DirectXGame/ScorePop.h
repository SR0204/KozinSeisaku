#pragma once
#include "KamataEngine.h"
#include <2d/Sprite.h>
#include <array>
#include <math/Vector2.h>

class ScorePop {
public:
	ScorePop(const KamataEngine::Vector2& screenPos, int score, const std::array<uint32_t, 10>& textures);

	void Update();
	void Draw(ID3D12GraphicsCommandList*); // ← ここ重要
	bool IsDead() const;

private:
	KamataEngine::Vector2 screenPos_;
	float timer_;
	float lifetime_;
	float yOffset_;
	int score_;
	std::array<uint32_t, 10> textures_;
};
