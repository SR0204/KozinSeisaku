#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


	// 初期化処理

	// エンジンの初期化
	KamataEngine::Initialize(L"LE3C_13_スズキレオ");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// DirectXCommonクラスが管理している、ウィンドウの幅と高さ
	int32_t w = dxCommon->GetBackBufferWidth();
	int32_t h = dxCommon->GetBackBufferHeight();
	DebugText::GetInstance()->ConsolePrintf(std::format("width:{},height:{}\n", w, h).c_str());

	// 更新処理
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// 描画開始
		dxCommon->PreDraw();

		// ここに描画処理を書く

		dxCommon->PostDraw();
	}

	// 解放処理

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}