#include "Scene.h"
using namespace KamataEngine;

void Scene::SetUp() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	Initialize();
}

void Scene::Initialize() {}
void Scene::Update() {}
void Scene::Draw() {}

void Scene::Delete() {}

void Scene::DrawImGui() {}
