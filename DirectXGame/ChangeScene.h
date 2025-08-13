// ChangeScene.h
#pragma once
#include <functional>

enum class ChangeState { None, FadeOut, Switch, FadeIn };

class ChangeScene {
public:
	void Start(std::function<void()> changeFunc, float fadeTime = 1.0f);
	void Update(float deltaTime);
	void Draw(); // α値を使ってフェード描画

	bool IsChanging() const { return state_ != ChangeState::None; }

private:
	ChangeState state_ = ChangeState::None;
	float timer_ = 0.0f;
	float fadeTime_ = 1.0f;
	std::function<void()> changeFunc_;
};
