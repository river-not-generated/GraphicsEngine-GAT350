#include "pch.h"
#include "Player.h"
#include "Engine.h"
#include "Bullet.h"
#include "Assets.h"
#include "SpaceGame.h"
#include "Core/Factory.h"
#include "Components/RigidBodyComponent.h"

#include <iostream>

FACTORY_REGISTER(Player)

void Player::Update(float dt) {

    // movement control for the player square (WASD or arrows)
    // nu::Vector2 force{ 0.0f, 0.0f };

    float thrust = 0.0f;
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W) || nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_UP)) {
        thrust = m_speed;
        nu::Particle particle;
        particle.position = m_transform.position;
        particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Get().GetRenderer());
        particle.lifespan = nu::RandomFloat(0.5f, 1.5f);
        particle.velocity = { nu::RandomFloat(-100.0f, 100.0f), nu::RandomFloat(-200.0f, 200.0f) };

        nu::Engine::Get().GetPS().AddParticle(particle);
    }
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S) || nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_DOWN)) {
        thrust = -m_speed;
    }

    float rotate = 0.0f;
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A) || nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_LEFT)) {
        rotate = -90.0f;
    }
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D) || nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_RIGHT)) {
        rotate = 90.0f;
    }

    SetRotation(m_transform.rotation + rotate * dt);
    
    auto physicsComponent = GetComponent<nu::PhysicsComponent>();
    if (physicsComponent) {
        nu::Vector2 forward{ 1.0f, 0.0f };
        nu::Vector2 force = forward.Rotate(m_transform.rotation * nu::math::DEG_TO_RAD) * thrust;

        physicsComponent->ApplyForce(force);
        physicsComponent->ApplyTorque(rotate);

        nu::Vector2 position = physicsComponent->GetPosition();

        position.x = nu::math::Wrap(0.0f, nu::Engine::Get().GetRenderer().GetWindowWidth(), position.x);
        position.y = nu::math::Wrap(0.0f, nu::Engine::Get().GetRenderer().GetWindowHeight(), position.y);

        physicsComponent->SetPosition(position);
    }
    
    // dash (broken)
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_LSHIFT) && m_dashCooldown == 0.0f) {
        thrust = m_speed * 125;
        for (int i = 0; i < 50; i++) {
            nu::Particle particle;
            particle.position = m_transform.position;
            particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Get().GetRenderer());
            particle.lifespan = nu::RandomFloat(0.5f, 1.5f);
            particle.velocity = { nu::RandomFloat(-100.0f, 100.0f), nu::RandomFloat(-200.0f, 200.0f) };

            nu::Engine::Get().GetPS().AddParticle(particle);
        }
        m_dashCooldown = 2.5f;
        nu::Engine::Get().GetAudio().PlaySound("thrust");
    }

    //SetVelocity(GetVelocity() + (force * dt));

    // fire bullet
    if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE)) {
        for (int i = 0; i < (m_tripleShot ? 3 : 1); i++) {
            float rotation = m_transform.rotation;
            switch (i) {
                case 1: 
                    rotation += 5.0f;
                    break;
                case 2: 
                    rotation -= 5.0f;
                    break;
            }
            auto bullet = nu::Factory::Instance().Create<Bullet>("BulletPrototype");
            bullet->SetPosition(m_transform.position);
            bullet->SetRotation(rotation);

            m_scene->AddActor(std::move(bullet));
        }
        nu::Engine::Get().GetAudio().PlaySound("fire");
    }
    m_powerupTime = std::max(0.0f, m_powerupTime - dt);
    m_dashCooldown = std::max(0.0f, m_dashCooldown - dt);
    if (m_tripleShot && m_powerupTime <= 0.0f) DeactivateTriple();

    Actor::Update(dt);
}

void Player::OnCollision(Actor* other) {
    return;
    if (other->GetTag() == "Enemy") {
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

        ((SpaceGame*)m_scene->GetGame())->OnPlayerDead();
    }

    if (other->GetTag() == "TripleShot") {
        ActivateTriple();
        other->Destroy();
        nu::Engine::Get().GetAudio().PlaySound("powerup");
    }
    if (other->GetTag() == "Heart") {
        ((SpaceGame*)m_scene->GetGame())->AddHeart();
        other->Destroy();
        nu::Engine::Get().GetAudio().PlaySound("powerup");
    }
}

void Player::Read(const nu::json::value_t& value) {
    Actor::Read(value);

    JSON_READ_NAME(value, "speed", m_speed);
}