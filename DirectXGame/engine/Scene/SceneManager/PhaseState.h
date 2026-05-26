#pragma once
#include "engine/Scene/SceneManager/SceneID.h"
#include "KamataEngine.h"
#include <optional>

// クラスの前方宣言（相互参照を防ぐため）
class PhaseManager;

class PhaseState {
public:
	virtual ~PhaseState() = default;

	// 各状態ごとの更新処理（SceneIDを返す可能性があるため std::optional）
	virtual std::optional<SceneID> Update(PhaseManager* context) = 0;

	// 各状態ごとの描画処理
	virtual void Draw(PhaseManager* context) = 0;
};