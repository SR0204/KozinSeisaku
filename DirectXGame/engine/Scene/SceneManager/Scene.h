#pragma once

class SceneManager;

class Scene {
public:
	virtual ~Scene() = default;
	virtual void Initialize(SceneManager* sceneManager) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	/// <summary>
	/// ImGui用
	/// </summary>
	virtual void DrawImGui() = 0;
};
