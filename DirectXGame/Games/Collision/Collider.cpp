#include "Collider.h"

void Collider::Initialize(ObjID objId, Sphere sphere, bool isWarp) {
	objID_ = objId;
	sphere_ = sphere;
	isWarp_ = isWarp;
}
