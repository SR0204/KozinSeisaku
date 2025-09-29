#pragma once
#include "../StageID.h"
#include "Game/LoadJsonFile/FileJson.h"
#include <KamataEngine.h>
#include <base/DirectXCommon.h>
#include <math/Vector3.h>

using namespace KamataEngine;

struct StageData {
	std::vector<std::vector<StageType>> data;
	KamataEngine::Vector2 blockSize;
};

class Stage {

public: // 関数
	void Initialize(int ereaNum_, int stageNum_, std::string stage_);

	void Update();

	void Draw(KamataEngine::Camera* camera_);

	/*uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }*/

	/*struct IndexSet {
	    uint32_t xIndex;
	    uint32_t yIndex;
	};*/

	// IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	// Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	// Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	// 範囲短形
	// struct Rect {
	//	float left;   // 左端
	//	float right;  // 右端
	//	float bottom; // 下端
	//	float top;    // 上端
	//};

	// getter
	std::vector<std::vector<StageType>> GetData() { return StageData_.data; }

	KamataEngine::Vector2 GetBlockSize() { return StageData_.blockSize; }

private: // 変数
	StageData StageData_;

	// Csvデータ
	std::vector<std::vector<int>> csvData_;

	// Json読み書き用のファイルアクセサ
	FileJson::FileAccessor* fileAccessor_;

	// モデル
	KamataEngine::Model* BlockModel_;

	// ワールドトランスフォーム
	std::vector<std::vector<WorldTransform>> worldTransform_;

	KamataEngine::Camera* camera;
};