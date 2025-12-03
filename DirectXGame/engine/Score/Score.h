#pragma once
#include "../../engine/LoadJson/nlohmann/json.hpp"
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
	void AddScore(int value);
	void Draw(int numberTextures[], float x, float y);

	int GetScore() const { return score; }

	int GetHighScore() const { return highScore; }

	void DrawHighScore(int numberTextures[], float x, float y); // ハイスコア描画
	void SaveHighScoreTxt();                                    // txt保存
	void LoadHighScoreTxt();                                    // txt読み込み
	void SaveHighScoreJson();                                   // json保存
	void LoadHighScoreJson();                                   // json読み込み

private:
	int score;

	int highScore;
};
