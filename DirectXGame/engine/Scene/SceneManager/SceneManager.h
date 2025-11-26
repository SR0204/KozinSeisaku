#pragma once
#include "Scene.h"
#include "SceneID.h"
#include <memory>
/// <summary>
/// シーン切り替えをするクラス
/// </summary>
class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void ChangeScene(SceneID nextScene);  // 実際に切り替える
	void RequestScene(SceneID nextScene); // Update 内で切り替え要求
	void Update();
	void Draw();

	void SetSelectedStage(int stage) { selectedStage_ = stage; }
	int GetSelectedStage() const { return selectedStage_; }

private:
	std::unique_ptr<Scene> currentScene_;
	SceneID currentID_;
	SceneID nextScene_ = SceneID::None; // 切り替えフラグ
	int selectedStage_ = 0;
};
