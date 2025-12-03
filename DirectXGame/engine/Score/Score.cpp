#include "Score.h"
#include <string>

using namespace KamataEngine;

Score::Score() : score(0) {}

void Score::Initialize() { score = 0; }

// 基本加算
void Score::AddScore(int value) { score += value; }

// 敵撃破専用：敵種別ごとに点数を付ける
void Score::AddEnemyScore(int enemyType) {

	int addValue = 0;

	switch (enemyType) {
	case 0: // 雑魚
		addValue = 100;
		break;
	case 1: // 中型
		addValue = 300;
		break;
	case 2: // ボス
		addValue = 1000;
		break;
	default:
		addValue = 50;
		break;
	}

	score += addValue;
}

void Score::Draw(int numberTextures[], float x, float y) {

	std::string scoreText = std::to_string(score);
	float offsetX = x;

	for (char c : scoreText) {
		int digit = c - '0';
		if (digit < 0 || digit > 9)
			continue;

		Sprite* digitSprite = Sprite::Create(numberTextures[digit], {offsetX, y});
		digitSprite->Draw();

		offsetX += 64; // １桁分ずらす
	}
}
