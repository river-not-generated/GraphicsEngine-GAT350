#pragma once
#include "Framework/Object.h"

namespace nu
{
	class Actor;

	class Component : public Object {
	public:
		CLASS_PROTOTYPE(Component)

		Component() = default;
		virtual ~Component() = default;

		virtual void OnStart() {}
		virtual void OnDestroy() {}

		virtual void Update(float dt) {}

		Actor* GetOwner() const { return m_owner; }
		void SetOwner(Actor* owner) { m_owner = owner; }

	protected:
		Actor* m_owner = nullptr;
	};
}