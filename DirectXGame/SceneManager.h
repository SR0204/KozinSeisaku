// SceneManager.h
#pragma once
#include <functional>
#include <string>
#include <unordered_map>

class SceneManager {
public:
	void AddScene(const std::string& name, std::function<void()> updateFunc, std::function<void()> drawFunc);

	void ChangeScene(const std::string& name);

	void Update();
	void Draw();

private:
	struct SceneData {
		std::function<void()> update;
		std::function<void()> draw;
	};
	std::unordered_map<std::string, SceneData> scenes_;
	SceneData* currentScene_ = nullptr;
};
