#pragma once
#include "CharacterBase.h"

class GoalController : public CharacterBase {
public:
	CLASS_PROTOTYPE(GoalController)
		void Update(float dt) override;

private:
};