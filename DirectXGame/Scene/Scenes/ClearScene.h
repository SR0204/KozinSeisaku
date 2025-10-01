#pragma once
#include "KamataEngine.h"
#include "Scene.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;


class ClearScene : public Scene {
public:
	ClearScene();
	~ClearScene();

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	uint32_t clearTextureHandle_ = 0;
	KamataEngine::Sprite* clearSprite_ = nullptr;

	int frameCount_ = 0;
};
