#pragma once
#include "PhysicsComponent.h"

namespace nu {
	class RigidBodyComponent : public PhysicsComponent {
	public:
		CLASS_PROTOTYPE(RigidBodyComponent)

		void Update(float dt) override;
		void Read(const json::value_t& value) override;

		void ApplyForce(const Vector2& force) override;

		Vector2 GetVelocity() override;
		void SetVelocity(const Vector2& velocity) override;

		void ApplyTorque(float torque) override;

		float GetAngularVelocity() const override;
		void SetAngularVelocity(float angularVelocity) override;

		Vector2 GetPosition() const override;
		void SetPosition(const Vector2& position) override;
		float GetRotation() const override;
		void SetRotation(float rotation) override;

	private:
		Vector2 m_acceleration{ 0.0f, 0.0f };
		Vector2 m_velocity{ 0, 0 };

		float m_angularAcceleration{ 0.0f };
		float m_angularVelocity = 0.0f;


	};
}