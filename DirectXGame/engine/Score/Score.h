#pragma once
#include "2d/Sprite.h"
#include "KamataEngine.h"
#include <string>

class Score {
public:
	Score();

	void Initialize();
	void AddScore(int value);
	void Draw(int numberTextures[], float x, float y);

	int GetScore() const { return score; }

private:
	int score;
};
