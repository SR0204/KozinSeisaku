#include "Pause.h"

using namespace KamataEngine;

void Pause::Initialize(SceneManager* sceneManager) {
	isPoused_ = false;
	input_ = Input::GetInstance();
	sceneManager_ = sceneManager;
}

void Pause::Update() {

	// ESCでポーズ切り替え
	if (input_->TriggerKey(DIK_ESCAPE)) {
		isPoused_ = !isPoused_;
	}

	// Tでタイトル移動
	if (isPoused_ && input_->TriggerKey(DIK_T)) {
		requestReturnTitle_ = true;
	}
}

void Pause::Draw() {
	if (!isPoused_)
		return;
}
