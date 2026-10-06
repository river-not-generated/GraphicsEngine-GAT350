#include "pch.h"
#include "CircleColliderComponent.h"
#include "Framework/Actor.h"
#include "Core/Factory.h"

namespace nu {
    FACTORY_REGISTER(CircleColliderComponent)

    bool CircleColliderComponent::CheckCollision(const ColliderComponent& other)
    {
        const CircleColliderComponent* circle = dynamic_cast<const CircleColliderComponent*>(&other);

        if (!circle) return false;

        float distance = GetOwner()->GetTransform().position.Distance(other.GetOwner()->GetTransform().position);
            
        return distance <= m_radius + circle->GetRadius();
    }

    void CircleColliderComponent::Read(const json::value_t& value)
    {
        ColliderComponent::Read(value);

        JSON_READ_NAME(value, "radius", m_radius);
    }
}
