#pragma once
#include "MapChipField.h"
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <math/Vector3.h>

#include <string>
#include <vector>

class MapManager {
public:
	MapManager();
	~MapManager();

	// マップチップCSVを読み込み
	void Load(const std::string& path);

	// ブロック生成（通常 or クリア）
	std::vector<std::vector<WorldTransform*>> GenerateBlockTransforms(MapChipType type);

	// プレイヤー・敵のスポーン位置取得
	Vector3 GetPlayerSpawnPos() const;
	Vector3 GetEnemySpawnPos() const;

	// MapChipField取得
	MapChipField* GetMapChipField() const;

private:
	MapChipField* mapChipField_;
};
