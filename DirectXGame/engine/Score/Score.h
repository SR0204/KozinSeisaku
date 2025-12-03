#pragma once
#include "2d/Sprite.h"
#include "KamataEngine.h"
#include <string>

/// <summary>
/// スコアを計算、描画を作るクラス
/// </summary>

class Score {
public:
	Score();

	void Initialize();

	/// <summary>
	/// スコア加算
	/// </summary>
	/// <param name="value"></param>
	void AddScore(int value);

	void Draw(int numberTextures[], float x, float y);

	int GetScore() const { return score; }

private:
	int score;
};
