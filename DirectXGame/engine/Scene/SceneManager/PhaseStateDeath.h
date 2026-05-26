#pragma once
#include "PhaseState.h"

class PhaseStateDeath : public PhaseState {
public:
	std::optional<SceneID> Update(PhaseManager* context) override;
	void Draw(PhaseManager* context) override;
};