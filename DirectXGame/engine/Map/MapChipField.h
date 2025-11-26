#pragma once
#include <KamataEngine.h>
#include <base/DirectXCommon.h>
#include <math/Vector3.h>

using namespace KamataEngine;

enum class MapChipType {
	kBlank,  // 空白
	kBlock,  // ブロック
	kPlayer, // プレイヤーのリス地
	kEnemy   // 敵のリス地
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップを作るサイズやcsvのロードをするクラス
/// </summary>

class MapChipField {

public: // 関数
	void ResetMapChipData();

	void LoadMapchipCsv(const std::string& filePath);

	MapChipType GetMapchipTypeByIndex(uint32_t xIndex, uint32_t yIndex) const;

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) const;

	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	// 範囲短形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex) const;

	// 指定インデックス位置の真下にあるブロックの上面Y座標を取得
	float GetBlockTopY(int xIndex, int yIndex) const;

	// ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const float kBlockDepth = 1.0f;

private: // 変数
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};