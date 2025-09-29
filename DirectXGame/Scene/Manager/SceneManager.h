#pragma once
#include "KamataEngine.h"
#include "StageID.h"
#include "stages/Stage.h"

class StageManager {

public:
	void Initialize(int ereaNum_, int stageNum_, std::string stage_);

	void Update();

	void Draw(KamataEngine::Camera* camera_);

	// getter
	std::vector<std::vector<StageType>> GetData() { return Stage_->GetData(); }
	KamataEngine::Vector2 GetBlockSize() { return Stage_->GetBlockSize(); }

private:
	// Csvデータ
	std::vector<std::vector<int>> csvData_;

	Stage* Stage_;
};
