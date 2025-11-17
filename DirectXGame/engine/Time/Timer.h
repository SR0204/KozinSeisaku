#pragma once

#include "KamataEngine.h"
#include <iostream>
#include <windows.h>

class GameScene;

class Timer {
public:
	// コンストラクタ
	Timer(float limit);

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(int numberTextures[], float x, float y);

	// 時間切れ判定
	bool IsTimeOver() const;

private:
	float limitTime;         // 制限時間（秒）
	LARGE_INTEGER startTime; // 開始時刻
	LARGE_INTEGER lastTime;  // 終了時刻
	LARGE_INTEGER frequency; // 高精度タイマーの周波数

	GameScene* gameScene_;

	float elapsedTime = 0.0f;
	bool isActive = false;
};