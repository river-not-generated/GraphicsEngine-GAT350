#include "pch.h"
#include "Enemy.h"
#include "Engine.h"
#include "Player.h"
#include "SpaceGame.h"
#include "Core/Factory.h"
#include "Components/RigidBodyComponent.h"
#include <iostream>

FACTORY_REGISTER(Enemy)

void Enemy::Update(float dt) {
    Player* player = m_scene->GetActorByTag<Player>("Player");
    if (player) {
        auto physicsComponent = GetComponent<nu::PhysicsComponent>();
        if (physicsComponent) {
            nu::Vector2 forward{ 1.0f, 0.0f };
            nu::Vector2 force = forward.Rotate(m_transform.rotation * nu::math::DEG_TO_RAD) * m_speed;

            physicsComponent->ApplyForce(force);

            nu::Vector2 direction = player->GetTransform().position - m_transform.position;
            float rotation = direction.Angle();
            physicsComponent->SetRotation(rotation * nu::math::RAD_TO_DEG);

            nu::Vector2 position = physicsComponent->GetPosition();

            position.x = nu::math::Wrap(0.0f, nu::Engine::Get().GetRenderer().GetWindowWidth(), position.x);
            position.y = nu::math::Wrap(0.0f, nu::Engine::Get().GetRenderer().GetWindowHeight(), position.y);

            physicsComponent->SetPosition(position);
        }
    }

    Actor::Update(dt);
}

void Enemy::OnCollision(Actor* other)
{
    if (other->GetTag() == "Bullet") {
        other->Destroy();
        m_destroyed = true;

        for (int i = 0; i < 100; i++)
        {
            nu::Particle particle;
            particle.position = m_transform.position;
            particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Get().GetRenderer());
            particle.lifespan = nu::RandomFloat(0.5f, 2.0f);
            particle.velocity = { nu::RandomFloat(-600.0f, 600.0f), nu::RandomFloat(-600.0f, 600.0f) };

            nu::Engine::Get().GetPS().AddParticle(particle);
        }
        nu::Engine::Get().GetAudio().PlaySound("kill");
        ((SpaceGame*)m_scene->GetGame())->AddPoints(100);
        ((SpaceGame*)m_scene->GetGame())->IncreaseKillCount();
        ((SpaceGame*)m_scene->GetGame())->UpdateSpawnTime();
    }
}

void Enemy::Read(const nu::json::value_t& value) {
    Actor::Read(value);

    JSON_READ_NAME(value, "speed", m_speed);
}
