#include "Stage.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

using namespace KamataEngine;

namespace {

std::map<std::string, StageType> mapChipTable = {
    {"0", StageType::kBlank         },
    {"1", StageType::kBlock         },
    {"2", StageType::kEntrancePortal},
    {"3", StageType::kExitPortal    },
};
}

void Stage::Initialize(int ereaNum_, int stageNum_, std::string stage_) {
	BlockModel_ = Model::CreateFromOBJ("block");

	// FileAccessorの初期化
	fileAccessor_ = nullptr;
	// JSONファイル名を指定してFileAccessorを初期化 (相対パスを使用)
	fileAccessor_ = new FileJson::FileAccessor("Resources/Json/Stage.json");

	csvData_ = fileAccessor_->ReadCsvData(stage_, std::to_string(ereaNum_) + "_" + std::to_string(stageNum_));
	StageData_.blockSize.x = fileAccessor_->Read(stage_, "blockSizeX", float());
	StageData_.blockSize.y = fileAccessor_->Read(stage_, "blockSizeY", float());

	// worldTransform をステージサイズにリサイズ
	worldTransform_.resize(csvData_.size());
	for (uint32_t y = 0; y < csvData_.size(); y++) {
		worldTransform_[y].resize(csvData_[y].size());
	}
	StageData_.data.resize(csvData_.size());

	// CSVからマップチップデータを読み込む
	for (uint32_t y = 0; y < csvData_.size(); y++) {
		StageData_.data[y].resize(csvData_[y].size());
		for (uint32_t x = 0; x < csvData_[y].size(); x++) {
			if (csvData_[y][x] == 0) {
				StageData_.data[y][x] = StageType::kBlank;
			} else {
				StageData_.data[y][x] = static_cast<StageType>(csvData_[y][x]);
			}
			Vector3 BlockPos = {1.0f * x, 1.0f * (csvData_.size() - 1 - y), 0};

			worldTransform_[y][x].translation_ = BlockPos;
			worldTransform_[y][x].Initialize();
		}
	}
}

void Stage::Update() {
	for (uint32_t y = 0; y < csvData_.size(); y++) {
		for (uint32_t x = 0; x < csvData_[y].size(); x++) {
			worldTransform_[y][x].UpdateMatrix();
		}
	}
}

void Stage::Draw(KamataEngine::Camera* camera_) {
	for (uint32_t y = 0; y < csvData_.size(); y++) {
		for (uint32_t x = 0; x < csvData_[y].size(); x++) {
			if (StageData_.data[y][x] == StageType::kBlock) {
				BlockModel_->Draw(worldTransform_[y][x], *camera_);
			}
		}
	}
}

// Vector3 Stage::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0); }
//
// Stage::IndexSet Stage::GetMapChipIndexSetByPosition(const Vector3& position) {
//
//	IndexSet indexSet = {};
//
//	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
//
//	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);
//
//	return indexSet;
// }
//
// Stage::Rect Stage::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
//	// 指定ブロックの中心座標を取得する
//	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
//
//	Rect rect;
//	rect.left = center.x - kBlockWidth / 2.0f;
//	rect.right = center.x + kBlockWidth / 2.0f;
//	rect.bottom = center.y - kBlockHeight / 2.0f;
//	rect.top = center.y + kBlockHeight / 2.0f;
//
//	return rect;
// }