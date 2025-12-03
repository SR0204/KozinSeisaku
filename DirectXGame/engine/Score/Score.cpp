#include "Score.h"
#include <string>

using namespace KamataEngine;

Score::Score() : score(0) {}

void Score::Initialize() { score = 0; }

void Score::AddScore(int value) {
	score += value;
	if (score > highScore) {
		highScore = score;
	}
}

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

// ハイスコア描画
void Score::DrawHighScore(int numberTextures[], float x, float y) {
	std::string scoreText = std::to_string(highScore);
	float offsetX = x;
	for (char c : scoreText) {
		int digit = c - '0';
		if (digit < 0 || digit > 9)
			continue;
		Sprite* digitSprite = Sprite::Create(numberTextures[digit], {offsetX, y});
		digitSprite->Draw();
		offsetX += 64;
	}
}

// TXT保存/読み込み
void Score::SaveHighScoreTxt() {
	std::ofstream file("highscore.txt");
	if (file.is_open())
		file << highScore;
}

void Score::LoadHighScoreTxt() {
	std::ifstream file("highscore.txt");
	if (file.is_open())
		file >> highScore;
	else
		highScore = 0;
}

// JSON保存/読み込み
void Score::SaveHighScoreJson() {
	json j;
	j["highScore"] = highScore;
	std::ofstream file("highscore.json");
	if (file.is_open())
		file << j.dump(4);
}

void Score::LoadHighScoreJson() {
	std::ifstream file("highscore.json");
	if (file.is_open()) {
		json j;
		file >> j;
		highScore = j.value("highScore", 0);
	} else {
		highScore = 0;
	}
}