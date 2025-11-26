// ParticleSystem.cpp
#include "ParticleSystem.h"
#include "../../etc/MathUtilityForText.h"
#include <cassert>

using namespace KamataEngine;

using namespace KamataEngine;

ParticleSystem::ParticleSystem(Camera* camera) : camera_(camera) {
	// 内部でモデル生成
	quadModel_ = Model::CreateFromOBJ("deathParticle", true);
}


void ParticleSystem::Initialize(int maxParticles) {
	maxParticles_ = maxParticles;
	particles_.reserve(maxParticles_);
}

void ParticleSystem::Emit(const Vector3& position, const Vector3& velocity, float lifetime) {
	if (particles_.size() >= maxParticles_)
		return;
	particles_.push_back({position, velocity, lifetime});
}

void ParticleSystem::Update() {
	for (auto it = particles_.begin(); it != particles_.end();) {
		it->position += it->velocity;
		it->lifetime -= 1.0f / 60.0f;
		if (it->lifetime <= 0) {
			it = particles_.erase(it);
		} else {
			++it;
		}
	}
}

void ParticleSystem::Draw() {
	for (const auto& p : particles_) {
		worldTransform_.Initialize();
		worldTransform_.translation_ = p.position;
		worldTransform_.scale_ = {0.3f, 0.3f, 0.3f};
		worldTransform_.UpdateMatrix();

		quadModel_->Draw(worldTransform_, *camera_);
	}
}
