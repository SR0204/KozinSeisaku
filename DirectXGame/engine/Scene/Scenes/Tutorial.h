#pragma once
#include "../../../engine/Player/Player.h"
#include "KamataEngine.h"

enum class FadeState { None, FadeIn, FadeOut };

class TutorialUI {
public:
	void Initialize();
	void Update();
	void Draw();

	void Show(); // フェードイン
	void Hide(); // フェードアウト
	bool IsVisible() const { return isVisible_; }

private:
	std::unique_ptr<Sprite> sprite_;

	bool isVisible_ = false;
	FadeState fadeState_ = FadeState::None;
	float alpha_ = 0.0f;
};
