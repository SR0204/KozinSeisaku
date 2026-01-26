#include "Timer.h"
#include "2d/Sprite.h"
#include <string>

using namespace KamataEngine;

// コンストラクタ：制限時間を設定
Timer::Timer(float limit) : limitTime(limit) {}

void Timer::Initialize() {
	// 現在時間の保存（高精度タイマーの開始）
	QueryPerformanceFrequency(&frequency); // タイマーの周波数を取得
	QueryPerformanceCounter(&startTime);   // 開始時刻を取得

	prevTime = startTime;
	elapsedTime = 0.0f;
	isActive = false; // ゲーム開始までは進めない
}

void Timer::Update() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	if (!isActive) {
		prevTime = currentTime;
		return;
	}
	float deltaTime = static_cast<float>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;

	elapsedTime += deltaTime;

	prevTime = currentTime;
}

void Timer::Draw(int numberTextures[], float x, float y) {

	float remainingTime = limitTime - elapsedTime;
	if (remainingTime < 0)
		remainingTime = 0;

	int displayTime = static_cast<int>(remainingTime);
	std::string timeText = std::to_string(displayTime);

	float offsetX = x;

	for (char c : timeText) {
		int digit = c - '0';
		if (digit < 0 || digit > 9)
			continue;

		Sprite* digitSprite = Sprite::Create(numberTextures[digit], {offsetX, y});
		digitSprite->Draw();

		offsetX += 64;
	}
}

bool Timer::IsTimeOver() const { return elapsedTime >= limitTime; }
