#define NOMINMAX
#include "../../DirectXGame/Scene/Scenes/TitleScene.h"
#include "KamataEngine.h"
#include "Scene/SceneManager/SceneManager.h"
#include "application/base/GameScene.h"
#include <Windows.h>
#include <iostream>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE3C_13_スズキレオ_ゴールへの大冒険");

	KamataEngine::DirectXCommon* dxCommon = KamataEngine::DirectXCommon::GetInstance();

	// ★ SceneManagerを作成
	SceneManager* sceneManager = new SceneManager();

	// ★ 最初のシーンをTitleSceneに設定
	sceneManager->ChangeScene(SceneID::TitleScene);

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// 現在のシーンを更新
		sceneManager->Update();

		// 描画開始
		dxCommon->PreDraw();

		// 現在のシーンを描画
		sceneManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// SceneManagerを解放
	delete sceneManager;
	sceneManager = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
