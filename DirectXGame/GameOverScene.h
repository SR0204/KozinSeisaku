#pragma once
#include "KamataEngine.h"
#include "Scene.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;


class GameOverScene : public Scene {
public:
	GameOverScene();
	~GameOverScene();

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	uint32_t gameOverTextureHandle_ = 0;
	KamataEngine::Sprite* gameOverSprite_ = nullptr;

	int frameCount_ = 0;
};
