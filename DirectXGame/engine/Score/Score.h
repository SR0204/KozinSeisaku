#pragma once
#include <2d/Sprite.h>
#include <string>

using namespace KamataEngine;

class Score {
public:
	Score();

	void Initialize();
	void AddScore(int value);          // 基本加算
	void AddEnemyScore(int enemyType); // 敵撃破専用加算
	void Draw(int numberTextures[], float x, float y);

	int GetScore() const { return score; }

private:
	int score;
};
