#include "MathUtilityForText.h"
#include <3d/Camera.h>
#include <KamataEngine.h>

// これがあると簡単
using namespace KamataEngine;

class Player;

struct Rect {
	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;
};

class CameraController {
public:
	void Initialize();
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	const Camera& GetViewProjection() const { return camera_; }
	void SetMovableArea(Rect area) { movableArea_ = area; }

	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 23.0f;
	static inline const Rect Margin = {-640.0, 640.0, -360.0, 360.0};

private:
	Camera camera_; // KamataEngine::Camera と認識される
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f};
	Rect movableArea_ = {0, 100, 0, 100};
	Vector3 targetcoordinates_;
};
