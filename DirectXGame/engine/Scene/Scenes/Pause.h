#pragma once
#include "../../../engine/Scene/SceneManager/SceneManager.h"
#include "2d/Sprite.h"
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <input/Input.h>
#include <math/Vector3.h>

enum class PauseFadeState { None, FadeIn, FadeOut };

enum class SceneFadeState { None, FadeOut };

class Pause {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SceneManager* sceneManager);

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

	/// <summary>
	/// タイトルに戻った時
	/// </summary>
	/// <returns></returns>
	bool IsReturnTitle() const { return requestReturnTitle_; }

private:
	KamataEngine::Input* input_ = nullptr;

	bool isPoused_ = false;
	SceneManager* sceneManager_ = nullptr;
	bool requestReturnTitle_ = false;
	KamataEngine::Sprite* PauseSprite_ = nullptr;

	// ポーズフェード用
	float alpha_ = 0.0f; // 0.0～1.0
	PauseFadeState fadeState_ = PauseFadeState::None;

	//ポーズ画面~タイトル用黒フェード
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	float fadeAlpha_ = 0.0f;
	SceneFadeState sceneFadeState_ = SceneFadeState::None;
};
