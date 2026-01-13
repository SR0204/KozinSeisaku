#pragma once
#include "../../../engine/Player/Player.h"
#include "KamataEngine.h"

class TutorialUI {
public:
	void Initialize(Camera* camera);
	void Update();
	void Draw();

	void Show(); // フェードイン
	void Hide(); // フェードアウト
	bool IsVisible() const { return isVisible_; }

private:
	Model* model_ = nullptr;
	WorldTransform transform_;
	Camera* camera_ = nullptr;

	float alpha_ = 0.0f;

	enum class FadeState { None, FadeIn, FadeOut };

	FadeState fadeState_ = FadeState::None;
	bool isVisible_ = false;
};
