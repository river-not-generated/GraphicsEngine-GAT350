#include "EnemyController.h"
#include "Core/Factory.h"
#include "Damager.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "PlatformerGame.h"
#include "Engine.h"
#include "PlayerController.h"

FACTORY_REGISTER(EnemyController)

void EnemyController::OnStart()
{
	Actor::OnStart();

	m_physicsComponent = GetComponent<nu::PhysicsComponent>();
	assert(m_physicsComponent);
	m_spriteAnimComponent = GetComponent<nu::SpriteAnimatorComponent>();
	assert(m_spriteAnimComponent);
}

void EnemyController::Update(float dt)
{
	m_attackCooldown = nu::math::Clamp(0.0f, 100.0f, m_attackCooldown - dt);
	switch (m_state)
	{
	case CharacterBase::State::Move:
	{
		PlayerController* player = m_scene->GetActorByTag<PlayerController>("Player");
		if (player) {
			nu::Vector2 direction = player->GetTransform().position - m_transform.position;

			m_physicsComponent->SetVelocity(direction * 0.25f);
			m_spriteAnimComponent->SetFlipH(direction.x < 0);

			if (abs(player->GetTransform().position.x - m_transform.position.x) < 200
				&& abs(player->GetTransform().position.y - m_transform.position.y) < 150
				&& m_attackCooldown <= 0.0f) {
				m_state = State::Attack;
				m_spriteAnimComponent->Play("attack");
				m_attackCooldown = 1.0f;
			}
		}
	}
		break;
	case CharacterBase::State::Airborne:
		break;
	case CharacterBase::State::Attack:
		if (m_spriteAnimComponent->IsAnimationDone()) {
			m_spriteAnimComponent->Play("idle");
			m_state = State::Move;

			// only spawn the attack thingy after the animation is done because telegraphing
			// (gives the player time to hit it)
			auto actor = nu::Factory::Instance().Create<Damager>("DamagerPrototype");
			actor->SetPosition({ GetTransform().position.x + (64 * (m_spriteAnimComponent->GetFlipH() ? -1 : 1)), GetTransform().position.y});
			actor->SetTag("EnemyAttack");
			m_scene->AddActor(std::move(actor));
		}
		break;
	case CharacterBase::State::Hit:
	{
		if (m_spriteAnimComponent->IsAnimationDone()) {
			if (m_health <= 0) {
				Explode();
			}
			else {
				m_state = State::Move;
				m_spriteAnimComponent->Play("idle");
			}
		}
	}
		break;
	case CharacterBase::State::Death:
		break;
	default:
		break;
	}

	CharacterBase::Update(dt);
}

void EnemyController::OnCollision(Actor* other)
{
	if (nu::EqualsIgnoreCase(other->GetTag(), "PlayerAttack") && m_state != State::Hit) {
		std::cout << "bat was hit" << std::endl;
		m_state = State::Hit;
		Damager* damager = dynamic_cast<Damager*>(other);
		if (damager) {
			m_health -= damager->GetDamage();
			m_spriteAnimComponent->Play("hurt");
		}
	}
}

void EnemyController::Read(const nu::json::value_t& value)
{
	CharacterBase::Read(value);
}

void EnemyController::Explode()
{
	Destroy();
	for (int i = 0; i < 250; i++)
	{
		// explode upon death
		nu::Particle particle;
		particle.position = m_transform.position;
		particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Get().GetRenderer());
		particle.lifespan = nu::RandomFloat(0.5f, 1.25f);
		particle.velocity = { nu::RandomFloat(-900.0f, 900.0f), nu::RandomFloat(-900.0f, 900.0f) };

		nu::Engine::Get().GetPS().AddParticle(particle);
	}
	nu::Engine::Get().GetAudio().PlaySound("explode");
}
