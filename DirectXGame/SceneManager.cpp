#include "SceneManager.h"
#include "ClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "TitleScene.h"

SceneManager::SceneManager() : currentID_(SceneID::None) { RequestScene(SceneID::TitleScene); }

SceneManager::~SceneManager() {}

void SceneManager::RequestScene(SceneID nextScene) { nextScene_ = nextScene; }

void SceneManager::ChangeScene(SceneID nextScene) {
	currentID_ = nextScene;

	switch (nextScene) {
	case SceneID::TitleScene:
		currentScene_ = std::make_unique<TitleScene>();
		break;
	case SceneID::Game:
		currentScene_ = std::make_unique<GameScene>();
		break;
	case SceneID::Clear:
		currentScene_ = std::make_unique<ClearScene>();
		break;
	case SceneID::GameOver:
		currentScene_ = std::make_unique<GameOverScene>();
		break;
	default:
		currentScene_.reset();
		break;
	}

	if (currentScene_) {
		currentScene_->Initialize(this);
	}
	nextScene_ = SceneID::None;
}

void SceneManager::Update() {
	// シーン切り替え要求がある場合
	if (nextScene_ != SceneID::None) {
		ChangeScene(nextScene_);
		nextScene_ = SceneID::None;
	}

	if (currentScene_) {
		currentScene_->Update();
	}
}

void SceneManager::Draw() {
	if (currentScene_) {
		currentScene_->Draw();
	}
}
