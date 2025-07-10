#include "MapManager.h"

using namespace KamataEngine;

MapManager::MapManager() { mapChipField_ = nullptr; }

MapManager::~MapManager() { delete mapChipField_; }

void MapManager::Load(const std::string& path) {
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapchipCsv(path);
}

std::vector<std::vector<WorldTransform*>> MapManager::GenerateBlockTransforms(MapChipType type) {
	constexpr uint32_t kNumBlockVertical = 20;   // ← あなたのプロジェクトに合わせて
	constexpr uint32_t kNumBlockHorizontal = 30; // ← これも合わせて

	std::vector<std::vector<WorldTransform*>> blockList(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		blockList[i].resize(kNumBlockHorizontal);
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapchipTypeByIndex(j, i) == type) {
				WorldTransform* transform = new WorldTransform();
				transform->Initialize();

				Vector3 position = mapChipField_->GetMapChipPositionByIndex(j, i);
				/*if (type == MapChipType::kClear) {
					position.y = 2.0f;
					transform->rotation_ = {0.0f, 90.0f, 0.0f};
				}*/

				transform->translation_ = position;
				blockList[i][j] = transform;
			}
		}
	}
	return blockList;
}

Vector3 MapManager::GetPlayerSpawnPos() const {
	Vector3 pos = mapChipField_->GetMapChipPositionByIndex(14, 3); // ← 必要なら外部指定に
	pos.y = 2.0f;
	return pos;
}

Vector3 MapManager::GetEnemySpawnPos() const {
	Vector3 pos = mapChipField_->GetMapChipPositionByIndex(5, 3); // ← これも必要に応じて
	pos.y = 2.0f;
	return pos;
}

MapChipField* MapManager::GetMapChipField() const { return mapChipField_; }
