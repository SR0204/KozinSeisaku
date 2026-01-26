#pragma once
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <input/Input.h>
#include <math/Vector3.h>

class Pause {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ポーズ中か判断
	/// </summary>
	/// <returns></returns>
	bool IsPause() const { return isPoused_; }

private:
	KamataEngine::Input* input_ = nullptr;

	bool isPoused_ = false;
};
