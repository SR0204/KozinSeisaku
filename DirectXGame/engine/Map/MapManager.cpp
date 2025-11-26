#include "MapManager.h"

using namespace KamataEngine;

MapManager::MapManager() { mapChipField_ = nullptr; }

MapManager::~MapManager() { delete mapChipField_; }

void MapManager::Load(const std::string& path) {
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapchipCsv(path);
}

std::vector<std::vector<WorldTransform*>> MapManager::GenerateBlockTransforms(MapChipType type) {
	constexpr uint32_t kNumBlockVertical = 200;
	constexpr uint32_t kNumBlockHorizontal = 300;

	std::vector<std::vector<WorldTransform*>> blockList(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		blockList[i].resize(kNumBlockHorizontal);
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapchipTypeByIndex(j, i) == type) {
				WorldTransform* transform = new WorldTransform();
				transform->Initialize();

				Vector3 position = mapChipField_->GetMapChipPositionByIndex(j, i);
				transform->translation_ = position;
				blockList[i][j] = transform;
			}
		}
	}
	return blockList;
}

// ===== プレイヤーの出現位置を「2」のチップから探す =====
Vector3 MapManager::GetPlayerSpawnPos() const {
	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtical(); y++) {
		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizontal(); x++) {
			if (mapChipField_->GetMapchipTypeByIndex(x, y) == MapChipType::kPlayer) {
				Vector3 pos = mapChipField_->GetMapChipPositionByIndex(x, y);
				pos.y += 1.0f; // 少し浮かせる
				return pos;
			}
		}
	}
	// 万が一見つからなかったときの保険
	return {0.0f, 2.0f, 0.0f};
}

// ===== 敵の出現位置を「3」のチップから探す =====
Vector3 MapManager::GetEnemySpawnPos() const {
	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtical(); y++) {
		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizontal(); x++) {
			if (mapChipField_->GetMapchipTypeByIndex(x, y) == MapChipType::kEnemy) {
				Vector3 pos = mapChipField_->GetMapChipPositionByIndex(x, y);
				pos.y += 1.0f;
				return pos;
			}
		}
	}
	return {0.0f, 2.0f, 0.0f};
}

MapChipField* MapManager::GetMapChipField() const { return mapChipField_; }
