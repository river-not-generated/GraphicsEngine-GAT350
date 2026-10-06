#pragma once
#include "CharacterBase.h"


namespace nu {
	class PhysicsComponent;
	class SpriteAnimatorComponent;
}

class EnemyController : public CharacterBase {
public:
	EnemyController() = default;

	CLASS_PROTOTYPE(EnemyController)

	void OnStart() override;
	void Update(float dt) override;
	void OnCollision(Actor* other) override;

	void Read(const nu::json::value_t& value) override;

	void Explode();

private:
	nu::PhysicsComponent* m_physicsComponent{ nullptr };
	nu::SpriteAnimatorComponent* m_spriteAnimComponent{ nullptr };
	float m_attackCooldown = 0.0f;
};