#include "ScorePop.h"
#include "etc/MathUtilityForText.h"
#include <algorithm>
#include <math.h>
#include <math/Matrix4x4.h>
#include <math/Vector3.h>

using namespace KamataEngine;

struct Vector4 {
	float x, y, z, w;
};

static KamataEngine::Vector4 TransformVec4(const KamataEngine::Vector4& v, const Matrix4x4& m) {
	KamataEngine::Vector4 r;
	r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	r.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
	return r;
}

static Vector3 WorldToScreen(const Vector3& world, Camera* camera) {
	// VP行列 = view × projection
	Matrix4x4 vp = MatrixMultiply(const_cast<Matrix4x4&>(camera->GetViewMatrix()), const_cast<Matrix4x4&>(camera->GetProjectionMatrix()));

	KamataEngine::Vector4 pos4 = {world.x, world.y, world.z, 1.0f};
	pos4 = TransformVec4(pos4, vp);

	// NDC化
	pos4.x /= pos4.w;
	pos4.y /= pos4.w;

	Vector3 screen;
	screen.x = (pos4.x * 0.5f + 0.5f) * 1280.0f;
	screen.y = (-pos4.y * 0.5f + 0.5f) * 720.0f;

	return screen;
}

ScorePop::ScorePop(const Vector2& screenPos, int score, const std::array<uint32_t, 10>& textures)
    : screenPos_(screenPos), timer_(0.0f), lifetime_(0.6f), yOffset_(0.0f), score_(score), textures_(textures) {}

void ScorePop::Update() {
	timer_ += 1.0f / 60.0f;
	yOffset_ += 0.5f;
}

void ScorePop::Draw(ID3D12GraphicsCommandList*) {
	// ここでは Sprite::PreDraw / PostDraw は呼ばない（呼び元で1回だけ行う）
	float y = screenPos_.y - yOffset_;

	int s = score_;
	std::vector<int> digits;
	if (s == 0)
		digits.push_back(0);
	while (s > 0) {
		digits.push_back(s % 10);
		s /= 10;
	}
	std::reverse(digits.begin(), digits.end());

	float scale = 0.7f;
	float w = 24.0f * scale;
	float h = 32.0f * scale;

	float totalWidth = static_cast<float>(digits.size()) * w;
	float startX = screenPos_.x - totalWidth * 0.5f;

	for (int d : digits) {
		uint32_t tex = textures_[d];
		if (tex == 0) {
			startX += w;
			continue;
		}

		// Sprite::Create は sCommandList_ を参照して Draw を行うので、
		// 呼び出し元で PreDraw が呼ばれている必要がある。
		Sprite* spr = Sprite::Create(tex, {0, 0});
		if (!spr) {
			startX += w;
			continue;
		}

		spr->SetAnchorPoint({0.5f, 0.5f});
		spr->SetPosition({startX, y});
		spr->SetSize({w, h});
		spr->Draw(); // 安全（呼び元で PreDraw 済み）
		delete spr;

		startX += w;
	}
}

bool ScorePop::IsDead() const { return timer_ >= lifetime_; }
