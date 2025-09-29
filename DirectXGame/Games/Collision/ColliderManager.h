#pragma once
#include "Collider.h"
#include "ObjID.h"
class ColliderManager {
public:
	void CheckAllCollisions();
	void AddCollider(Collider* collider) { colliders_.push_back(collider); }
	void CollidersClear() { colliders_.clear(); }

private:
	std::list<Collider*> colliders_;
};
	