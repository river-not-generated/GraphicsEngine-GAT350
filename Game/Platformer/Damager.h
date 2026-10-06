#pragma once
#include "Framework/Actor.h"

class Damager : public nu::Actor
{
public:
	CLASS_PROTOTYPE(Damager)

	void Read(const nu::json::value_t& value) override;
	int GetDamage() const { return m_damage; }
	void AddDamage(int amount) { m_damage += amount; }

private:
	int m_damage = 1;
};