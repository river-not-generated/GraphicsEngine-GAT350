#pragma once
#include "Framework/Actor.h"

class CharacterBase : public nu::Actor
{
public:
	enum class State {
		Spawn,
		Move,
		Airborne,
		Attack,
		Hit,
		Death
	};

public:
	void Read(const nu::json::value_t& value) override;
	void Update(float dt) override;

	int GetHealth() { return m_health; }
	void SetHealth(int health) { m_health = health; }

protected:
	State m_state = State::Move;
	float m_stateTimer = 0.0f;
	int m_health = 10;
};