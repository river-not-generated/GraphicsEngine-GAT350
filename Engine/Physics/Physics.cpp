#include "pch.h"
#include "Physics.h"
#include "Framework/Actor.h"

namespace nu
{
	float Physics::m_pixelsPerUnit = 48.0f;

	bool Physics::Initialize()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, 10.0f };
		m_worldId = b2CreateWorld(&worldDef);

		return true;
	}

	void Physics::Shutdown()
	{
		b2DestroyWorld(m_worldId);
	}

	void Physics::Update(float dt)
	{
		b2World_Step(m_worldId, 1.0f / 60.0f, 4);
		ProcessCollisionEvents();
	}

	void Physics::ProcessCollisionEvents()
	{
		auto contactEvents = b2World_GetContactEvents(m_worldId);

		// begin contact
		for (int i = 0; i < contactEvents.beginCount; i++) {
			auto event = contactEvents.beginEvents + i;
			if (!(b2Shape_IsValid(event->shapeIdA) && b2Shape_IsValid(event->shapeIdB))) continue;

			b2BodyId bodyA = b2Shape_GetBody(event->shapeIdA);
			Actor* actorA = (Actor*) b2Body_GetUserData(bodyA);
			if (!actorA || actorA->IsDestroyed() || !actorA->IsActive()) continue;

			b2BodyId bodyB = b2Shape_GetBody(event->shapeIdB);
			Actor* actorB = (Actor*) b2Body_GetUserData(bodyB);
			if (!actorB || actorB->IsDestroyed() || !actorB->IsActive()) continue;

			actorA->OnCollision(actorB);
			actorB->OnCollision(actorA);
		}

		// begin sensor
		b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldId);
		for (int i = 0; i < sensorEvents.beginCount; i++) {
			auto event = sensorEvents.beginEvents + i;
			if (!(b2Shape_IsValid(event->sensorShapeId) && b2Shape_IsValid(event->visitorShapeId))) continue;

			b2BodyId bodyA = b2Shape_GetBody(event->sensorShapeId);
			Actor* actorA = (Actor*)b2Body_GetUserData(bodyA);
			if (!actorA || actorA->IsDestroyed() || !actorA->IsActive()) continue;

			b2BodyId bodyB = b2Shape_GetBody(event->visitorShapeId);
			Actor* actorB = (Actor*)b2Body_GetUserData(bodyB);
			if (!actorB || actorB->IsDestroyed() || !actorB->IsActive()) continue;

			actorA->OnCollision(actorB);
			actorB->OnCollision(actorA);
		}
	}
}
