#pragma once
#include "KamataEngine.h"
#include "Scene.h"

class DirectXCommon; // 前方宣言
class Input;         // 前方宣言
class Sprite;        // 前方宣言
class SceneManager;

class TitleScene : public Scene {
public:
	TitleScene();
	~TitleScene();

	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw() override;

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	uint32_t TitleTextureHandle_ = 0;
	uint32_t TitleTextureHandle2_ = 0;
	uint32_t TitleBackGroundTextureHandle_ = 0;
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* sprite2_ = nullptr;
	KamataEngine::Sprite* BackGround_ = nullptr;

	bool isTitle_ = true;
	int frameCount_ = 0;

	SceneManager* sceneManager_ = nullptr;

	uint32_t bgmHandle_ = 0;  // BGMのハンドル
	int bgmVoiceHandle_ = -1; // 再生中のBGMの音声ハンドル
	float bgmVolume_ = 1.0f;  // 現在のボリューム(1.0=最大)
	bool isFadingOut_ = false; // フェードアウト中かどうか
};
