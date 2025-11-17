#include "Score.h"
#include <string>

using namespace KamataEngine;

Score::Score() : score(0) {}

void Score::Initialize() { score = 0; }

void Score::AddScore(int value) { score += value; }

void Score::Draw(int numberTextures[], float x, float y) {

	std::string scoreText = std::to_string(score);

	float offsetX = x;

	// 数字を1桁ずつ描画
	for (char c : scoreText) {
		int digit = c - '0';

		if (digit < 0 || digit > 9) {
			continue;
		}

		Sprite* digitSprite = Sprite::Create(
		    numberTextures[digit], // 数字画像
		    {offsetX, y}           // 座標
		);

		digitSprite->Draw();

		offsetX += 64; // 次の桁の位置
	}
}
