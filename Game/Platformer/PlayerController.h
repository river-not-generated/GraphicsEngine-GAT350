#pragma once
#include "CharacterBase.h"

namespace nu {
    class PhysicsComponent;
    class SpriteAnimatorComponent;
}

class PlayerController : public CharacterBase
{
public:
    CLASS_PROTOTYPE(PlayerController)
    void OnStart() override;
    void Update(float dt) override;
    void OnCollision(nu::Actor* other) override;
    void Read(const nu::json::value_t& value) override;

protected:
    nu::PhysicsComponent* m_physicsComponent{ nullptr };
    nu::SpriteAnimatorComponent* m_spriteAnimComponent{ nullptr };
    float m_dir = 1.0f;
    int m_speed = 100;
    float m_jumpVelocity = 100.0f;
    float m_comboTimer = 0.0f;
    int m_combo = 0;
    float m_itime = 0.0f;

    int m_knockback = 75;
};
