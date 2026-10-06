#pragma once

#include "Framework/Actor.h"

struct EnemyDesc : public nu::ActorDesc {
	float speed;
};

class Enemy : public nu::Actor {
public:
	CLASS_PROTOTYPE(Enemy)

	Enemy() = default;
	Enemy(const EnemyDesc& enemyDesc) : Actor{ enemyDesc }, m_speed{ enemyDesc.speed } {}

	void Update(float dt) override;
	void OnCollision(Actor* other) override;

	float GetSpeed() { return m_speed; }

	void Read(const nu::json::value_t& value) override;


private:
	float m_speed = 400.0f;
};