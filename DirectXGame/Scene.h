#pragma once
#include "KamataEngine.h"
#include "SceneID.h"
class SceneManager;
class Scene {
public:
	virtual ~Scene() = default;
	/// <summary>
	/// Sceneクラスのセットアップ
	/// </summary>
	void SetUp();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();
	/// <summary>
	/// 削除
	/// </summary>
	virtual void Delete();
	/// <summary>
	/// ImGui描画
	/// </summary>
	virtual void DrawImGui();

	// シーン終了
	bool IsFinish() { return isFinish; }

	// 追加：終了したら次に遷移するシーンID
	virtual SceneID NextScene() const = 0;

	// setter
	void SetEreaNum(int num) { ereaNum = num; }
	void SetStageNum(int num) { stageNum = num; }
	void SetStage(std::string str) { stage = str; }
	void SetIsCrear(bool is) { isClear = is; }
	void SetIs1stPortalThrough(bool is) { is1stPortalThrough = is; }
	void SetIs2ndPortalThrough(bool is) { is2ndPortalThrough = is; }

	// getter
	int GetEreaNum() { return ereaNum; }
	int GetStageNum() { return stageNum; }
	std::string GetStage() { return stage; }
	bool GetIsClear() { return isClear; }
	bool GetIs1stPortalThrough() { return is1stPortalThrough; }
	bool GetIs2ndPortalThrough() { return is2ndPortalThrough; }

private:
private:
protected:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	// stageの数値と文字列
	int ereaNum;
	int stageNum;
	std::string stage = "Tutorial";

	bool isClear = true;
	bool is1stPortalThrough = false;
	bool is2ndPortalThrough = false;

	bool isFinish;
};