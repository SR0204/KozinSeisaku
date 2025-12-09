#include "ScoreManager.h"

using namespace KamataEngine;

ScoreManager& ScoreManager::Instance() {
	static ScoreManager instance;
	return instance;
}

void ScoreManager::Initialize(int stageIndex) {
	currentStage = stageIndex; // 0〜2
	score.Initialize();
	scoreLog.Load();
}

void ScoreManager::AddEnemyScore(int enemyType) { score.AddEnemyScore(enemyType); }

void ScoreManager::AddScore(int value) { score.AddScore(value); }

// 現在スコアをハイスコアに反映
void ScoreManager::UpdateHighScore() {
	int now = score.GetScore();
	int prev = scoreLog.GetHighScore(currentStage);

	if (now > prev) {
		scoreLog.SetHighScore(currentStage, now);
	}
}

void ScoreManager::Save() { scoreLog.Save(); }

int ScoreManager::GetScore() const { return score.GetScore(); }

int ScoreManager::GetHighScore() const { return scoreLog.GetHighScore(currentStage); }
