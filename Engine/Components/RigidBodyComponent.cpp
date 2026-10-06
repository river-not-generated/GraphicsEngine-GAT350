#include "pch.h"
#include "RigidBodyComponent.h"
#include "Core/Factory.h"
#include "Framework/Actor.h"


namespace nu
{
	FACTORY_REGISTER(RigidBodyComponent)

	void RigidBodyComponent::Update(float dt)
	{
		// force = mass * acceleration
		// acceleration = force / mass
		// velocity = velocity + acceleration
		// position = position + velocity

		m_velocity += m_acceleration * dt;
		m_velocity *= (1.0f / ((1.0f) + m_damping * dt));

		m_angularVelocity += m_angularAcceleration * dt;
		m_angularVelocity *= (1.0f / ((1.0f) + m_angularDamping * dt));

		Vector2 position = GetOwner()->GetTransform().position;
		position += m_velocity * dt;
		GetOwner()->SetPosition(position);

		float rotation = GetOwner()->GetTransform().rotation;
		rotation += m_angularVelocity * dt;
		GetOwner()->SetRotation(rotation);

		m_acceleration = Vector2{ 0.0f };
		m_angularAcceleration = 0.0f;

	}
	void RigidBodyComponent::Read(const json::value_t& value)
	{
		PhysicsComponent::Read(value);
		JSON_READ_NAME(value, "velocity", m_velocity);
		JSON_READ_NAME(value, "angular_velocity", m_angularVelocity);
	}

	void RigidBodyComponent::ApplyForce(const Vector2& force)
	{
		m_acceleration += force / m_mass;
	}
	Vector2 RigidBodyComponent::GetVelocity()
	{
		return m_velocity;
		
	}
	void RigidBodyComponent::SetVelocity(const Vector2& velocity)
	{
		m_velocity = velocity;
	}

	void RigidBodyComponent::ApplyTorque(float torque)
	{
		m_angularAcceleration += torque / m_mass;
	}

	float RigidBodyComponent::GetAngularVelocity() const
	{
		return m_angularVelocity;
	}
	void RigidBodyComponent::SetAngularVelocity(float angularVelocity)
	{
		m_angularVelocity = angularVelocity;
	}

	Vector2 RigidBodyComponent::GetPosition() const
	{
		return GetOwner()->GetTransform().position;
	}
	void RigidBodyComponent::SetPosition(const Vector2& position)
	{
		GetOwner()->SetPosition(position);
	}

	float RigidBodyComponent::GetRotation() const
	{
		return GetOwner()->GetTransform().rotation;
	}
	void RigidBodyComponent::SetRotation(float rotation)
	{
		GetOwner()->SetRotation(rotation);
	}
}