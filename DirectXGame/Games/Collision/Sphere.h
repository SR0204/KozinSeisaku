#pragma once
#include "KamataEngine.h"
struct Sphere {
	KamataEngine::Vector3 pos;
	float radius;
};

bool CollisionDetection(Sphere a, Sphere b);