// SceneManager.cpp
#include "SceneManager.h"
#include <iostream>

void SceneManager::AddScene(const std::string& name, std::function<void()> updateFunc, std::function<void()> drawFunc) { scenes_[name] = {updateFunc, drawFunc}; }

void SceneManager::ChangeScene(const std::string& name) {
	auto it = scenes_.find(name);
	if (it != scenes_.end()) {
		currentScene_ = &it->second;
	}
}

void SceneManager::Update() {
	if (currentScene_ && currentScene_->update)
		currentScene_->update();
}

void SceneManager::Draw() {
	if (currentScene_ && currentScene_->draw)
		currentScene_->draw();
}
