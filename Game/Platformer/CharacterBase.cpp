#include "CharacterBase.h"

void CharacterBase::Read(const nu::json::value_t& value)
{
	nu::Actor::Read(value);

	JSON_READ_NAME(value, "health", m_health);
}

void CharacterBase::Update(float dt)
{
	Actor::Update(dt);
}
