#include "ScoreLog.h"
#include <fstream>

ScoreLog::ScoreLog() { highScores.resize(3, 0); }

void ScoreLog::Load() {
	std::ifstream file("highScore.json");
	if (!file.is_open())
		return;

	json j;
	file >> j;

	for (int i = 0; i < 3; i++) {
		highScores[i] = j.value("stage" + std::to_string(i + 1), 0);
	}
}

void ScoreLog::Save() {
	json j;

	for (int i = 0; i < 3; i++) {
		j["stage" + std::to_string(i + 1)] = highScores[i];
	}

	std::ofstream file("highScore.json");
	file << j.dump(4);
}

int ScoreLog::GetHighScore(int stage) const { return highScores[stage]; }

void ScoreLog::SetHighScore(int stage, int score) {
	if (score > 999)
		score = 999;
	highScores[stage] = score;
}
