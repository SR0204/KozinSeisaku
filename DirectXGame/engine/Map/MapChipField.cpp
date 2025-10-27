#include "MapChipField.h"
#include "etc/MathUtilityForText.h"
#include <2d/DebugText.h> // DebugText を使う場合
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

using namespace KamataEngine;

namespace {

// CSV文字列 → MapChipType の変換テーブル
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank        },
    {"1", MapChipType::kBlock        },
    {"2", MapChipType::BreakableBlock},
    {"3", MapChipType::ItemBlock3    },
};

} // namespace

// マップチップデータをリセット
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical); // 縦の行数に合わせる
	for (auto& line : mapChipData_.data) {
		line.resize(kNumBlockHorizontal, MapChipType::kBlank);
	}
}

// CSVからマップを読み込む
void MapChipField::LoadMapchipCsv(const std::string& filePath) {
	ResetMapChipData();

	std::ifstream file(filePath);
	assert(file.is_open());

	std::string line;
	for (uint32_t y = 0; y < kNumBlockVirtical; ++y) {
		if (!std::getline(file, line))
			break;
		std::istringstream lineStream(line);
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			std::string word;
			if (!std::getline(lineStream, word, ','))
				break;
			if (mapChipTable.contains(word)) {
				mapChipData_.data[y][x] = mapChipTable[word];
			}
		}
	}

	file.close();
}

// インデックスからマップチップの種類を取得
MapChipType MapChipField::GetMapchipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical)
		return MapChipType::kBlank;
	return mapChipData_.data[yIndex][xIndex];
}

// インデックス → ワールド座標
Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0); }

// ワールド座標 → インデックス
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet{};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);
	return indexSet;
}

// 指定ブロックの範囲矩形を取得
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}

// プレイヤーが下から叩いたときの処理
void MapChipField::OnHitFromBelow(uint32_t xIndex, uint32_t yIndex) {
	MapChipType type = GetMapchipTypeByIndex(xIndex, yIndex);

	switch (type) {
	case MapChipType::BreakableBlock:
		// 壊す
		mapChipData_.data[yIndex][xIndex] = MapChipType::kBlank;
		DebugText::GetInstance()->ConsolePrintf("BreakableBlock destroyed at (%u,%u)\n", xIndex, yIndex);
		break;

	case MapChipType::ItemBlock3:
		// アイテムを出す
		SpawnItem(xIndex, yIndex);
		// ブロックを叩かれた状態に変更（色変えなど）
		mapChipData_.data[yIndex][xIndex] = MapChipType::kBlock;
		break;

	default:
		break;
	}
}

// アイテム生成処理
void MapChipField::SpawnItem(uint32_t xIndex, uint32_t yIndex) {
	Vector3 pos = GetMapChipPositionByIndex(xIndex, yIndex) + Vector3(0, kBlockHeight / 2.0f, 0);
	DebugText::GetInstance()->ConsolePrintf("Spawn Item at (%.2f, %.2f)\n", pos.x, pos.y);
	// TODO: ここでアイテムクラスを生成してゲームに追加する
}
