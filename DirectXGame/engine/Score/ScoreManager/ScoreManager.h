#pragma once
#include "../../../engine/Score/Log/ScoreLog.h"
#include "../../../engine/Score/Score.h"
#include "KamataEngine.h"

class ScoreManager {
public:
	static ScoreManager& Instance();

	void Initialize(int stageIndex); // ステージ番号 0〜2
	void AddEnemyScore(int enemyType);
	void AddScore(int value);

	void UpdateHighScore();
	void Save();

	int GetScore() const;
	int GetHighScore() const;

private:
	ScoreManager() = default;

	Score score;
	ScoreLog scoreLog;

	int currentStage = 0;
};
