#pragma once
#include "Games/Collision/Collider.h"
#include "Games/Collision/ObjID.h"
class ColliderManager {
public:
	void CheckAllCollisions();
	void AddCollider(Collider* collider) { colliders_.push_back(collider); }
	void CollidersClear() { colliders_.clear(); }

private:
	std::list<Collider*> colliders_;
};
	