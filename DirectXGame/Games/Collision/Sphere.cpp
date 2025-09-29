#include "Sphere.h"
using namespace KamataEngine;
using namespace MathUtility;

bool CollisionDetection(Sphere a, Sphere b) {
	float len = Length(a.pos - b.pos);

	if (len <= a.radius + b.radius) {
		return true;
	}
	return false;
}
