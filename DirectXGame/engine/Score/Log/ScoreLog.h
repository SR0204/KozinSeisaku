#pragma once
#include "../../../engine/LoadJson/nlohmann/json.hpp"
#include <string>
#include <vector>

using json = nlohmann::json;

class ScoreLog {
public:
	ScoreLog();

	void Load();
	void Save();

	int GetHighScore(int stage) const;
	void SetHighScore(int stage, int score);

private:
	std::vector<int> highScores; // ステージ別（3ステージ）
};
