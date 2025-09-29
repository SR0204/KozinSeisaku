#include "StageManager.h"

using namespace KamataEngine;

void StageManager::Initialize(int ereaNum_, int stageNum_, std::string stage_) {

	Stage_ = new Stage();
	Stage_->Initialize(ereaNum_, stageNum_, stage_);
}

void StageManager::Update() { Stage_->Update(); }

void StageManager::Draw(KamataEngine::Camera* camera_) { Stage_->Draw(camera_); }
