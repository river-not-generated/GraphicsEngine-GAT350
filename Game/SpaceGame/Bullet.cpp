#include "pch.h"
#include "Bullet.h"
#include "Engine.h"
#include "Player.h"
#include "Core/Factory.h"
#include "Components/RigidBodyComponent.h"

FACTORY_REGISTER(Bullet)

void Bullet::Update(float dt) {
    auto physicsComponent = GetComponent<nu::PhysicsComponent>();
    if (physicsComponent) {
        nu::Vector2 forward{ 1.0f, 0.0f };
        nu::Vector2 force = forward.Rotate(m_transform.rotation * nu::math::DEG_TO_RAD) * m_speed;

        physicsComponent->SetVelocity(force);

        nu::Vector2 position = physicsComponent->GetPosition();

        position.x = nu::math::Wrap(0.0f, nu::Engine::Get().GetRenderer().GetWindowWidth(), position.x);
        position.y = nu::math::Wrap(0.0f, nu::Engine::Get().GetRenderer().GetWindowHeight(), position.y);

        physicsComponent->SetPosition(position);
    }

    Actor::Update(dt);
}

void Bullet::Read(const nu::json::value_t& value) {
    Actor::Read(value);

    JSON_READ_NAME(value, "speed", m_speed);
}