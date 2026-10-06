#include "PlayerController.h"
#include "Core/StringUtils.h"
#include "Core/Factory.h"
#include "Engine.h"
#include "Math/Vector2.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Damager.h"
#include "Components/Box2DPhysicsComponent.h"
#include "PlatformerGame.h"

FACTORY_REGISTER(PlayerController)

void PlayerController::OnStart()
{
	Actor::OnStart();

	m_state = State::Spawn;
	m_physicsComponent = GetComponent<nu::PhysicsComponent>();
	assert(m_physicsComponent);
	m_spriteAnimComponent = GetComponent<nu::SpriteAnimatorComponent>();
	assert(m_spriteAnimComponent);
}

void PlayerController::Update(float dt)
{
	CharacterBase::Update(dt);
	nu::Vector2 velocity = m_physicsComponent->GetVelocity();
	nu::Vector2 prevPos = GetTransform().position;
	float dir = 0;
	// reduce invincibility timer and combo timer
	m_comboTimer = nu::math::Clamp(0.0f, 1.0f, m_comboTimer - dt);
	m_itime = nu::math::Clamp(0.0f, 2.0f, m_itime - dt);
	// reset combo if applicable
	if (m_comboTimer == 0.0f) m_combo = 0;

	switch (m_state)
	{
	case CharacterBase::State::Spawn:
		m_spriteAnimComponent->Play("fall");
		break;
	case CharacterBase::State::Move:
		if (m_health <= 0) {
			m_state = State::Death;
			m_spriteAnimComponent->Play("dead");
			((PlatformerGame*)m_scene->GetGame())->OnPlayerDead();
			break;
		}
		if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE)) {
			velocity.y = -m_jumpVelocity;
			nu::Engine::Get().GetAudio().PlaySound("jump");
			m_spriteAnimComponent->Play("jump");
			m_state = State::Airborne;
		}
		// spawn attack hitbox if attacking
		if (nu::Engine::Get().GetInput().GetMousePressed(nu::Input::MouseButton::Left)) {
			m_state = State::Attack;
			m_comboTimer = 0.75f;
			m_spriteAnimComponent->Play("attack" + std::to_string(m_combo % 3 + 1));
			nu::Engine::Get().GetAudio().PlaySound("attack");
			
			auto actor = nu::Factory::Instance().Create<Damager>("DamagerPrototype");
			actor->SetPosition({ GetTransform().position.x + (64 * m_dir), GetTransform().position.y });
			actor->SetTag("PlayerAttack");
			actor->GetComponent<nu::Box2DPhysicsComponent>()->SetSize({ 96.0f, 96.0f });
			m_scene->AddActor(std::move(actor));
			
			m_combo++;
			break;
		}
		[[fallthrough]];
	case CharacterBase::State::Airborne:
		if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) {
			dir = -1.0f;
			m_dir = -1.0f;
		}
		if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) {
			dir = 1.0f;
			m_dir = 1.0f;
		}
		if (dir != 0) {
			velocity.x = dir * m_speed;
			if (!nu::EqualsIgnoreCase(m_spriteAnimComponent->GetAnimationName(), "run") && m_state == State::Move) m_spriteAnimComponent->Play("run");
		}
		else {
			if (!nu::EqualsIgnoreCase(m_spriteAnimComponent->GetAnimationName(), "idle") && m_state == State::Move) m_spriteAnimComponent->Play("idle");
		}
		break;
	case CharacterBase::State::Attack:
		if (m_spriteAnimComponent->IsAnimationDone()) m_state = State::Move;
		break;
	case CharacterBase::State::Hit:
		// take knockback upon getting hit (velocity decided by animation progress so it's more slidey)
		velocity.x = -m_dir * (m_knockback * (m_spriteAnimComponent->GetTotalFrames() - m_spriteAnimComponent->GetFrame()));
		if (m_spriteAnimComponent->IsAnimationDone()) 
			if (m_health <= 0) {
				m_state = State::Death;
				m_spriteAnimComponent->Play("dead");
				((PlatformerGame*)m_scene->GetGame())->OnPlayerDead();
			}
			else { 
				m_state = State::Move; 
			}
		break;
	case CharacterBase::State::Death:
		break;
	default:
		break;

	}
	if (GetTransform().position.y > nu::Engine::Get().GetRenderer().GetWindowHeight()) {
		m_health--;
		nu::Engine::Get().GetAudio().PlaySound("fall");
		nu::Engine::Get().GetAudio().PlaySound("hurt");
		((PlatformerGame*)m_scene->GetGame())->SetHealth(m_health);
		Destroy();
	}

	m_spriteAnimComponent->SetFlipH(m_dir <= 0.0f);

	m_physicsComponent->SetVelocity(velocity);
	//nu::Engine::Get().GetRenderer().SetCamera(m_physicsComponent->GetPosition());

	nu::Vector2 currentPos = GetTransform().position;
	if (currentPos.y - prevPos.y > 2.0f && m_state == State::Move) { 
		m_spriteAnimComponent->Play("fall"); 
		m_state = State::Airborne;
	}
}

void PlayerController::OnCollision(Actor* other)
{
	if (nu::EqualsIgnoreCase(other->GetTag(), "Goal")) {
		if (m_state != State::Death) {
			((PlatformerGame*)m_scene->GetGame())->EndLevel();
			((PlatformerGame*)m_scene->GetGame())->SetHealth(m_health);
			nu::Engine::Get().GetAudio().PlaySound("level");
		}
	}
	if (nu::EqualsIgnoreCase(other->GetTag(), "Ground")) {
		if (m_state == State::Airborne || m_state == State::Spawn) {
			m_state = State::Move;
			m_spriteAnimComponent->Play("idle");
		}
	}
	if (nu::EqualsIgnoreCase(other->GetTag(), "EnemyAttack") && m_state != State::Death) {
		if (m_itime <= 0.0f) {
			
			m_health--;
			((PlatformerGame*)m_scene->GetGame())->SetHealth(m_health);
			m_itime = 1.0f;
			m_state = State::Hit;
			nu::Engine::Get().GetAudio().PlaySound("hurt");
			m_spriteAnimComponent->Play("hurt");

		}
	}
}

void PlayerController::Read(const nu::json::value_t& value)
{
	CharacterBase::Read(value);
	JSON_READ_NAME_REQ(value, "speed", m_speed);
	JSON_READ_NAME_REQ(value, "jump", m_jumpVelocity);
	JSON_READ_NAME(value, "knockback", m_knockback);
}
