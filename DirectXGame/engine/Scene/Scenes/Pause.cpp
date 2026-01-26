#include "Pause.h"

using namespace KamataEngine;

void Pause::Initialize() {
	isPoused_ = false;
	input_ = Input::GetInstance();
}

void Pause::Update() {

	// ESCでポーズ切り替え
	if (input_->TriggerKey(DIK_ESCAPE)) {
		isPoused_ = !isPoused_;
	}
}

void Pause::Draw() {
	if (!isPoused_)
		return;
}
