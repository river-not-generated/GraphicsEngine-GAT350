#include "GoalController.h"
#include "Engine.h"

FACTORY_REGISTER(GoalController)

void GoalController::Update(float dt)
{
	CharacterBase::Update(dt);
	nu::Particle particle;
	particle.position = m_transform.position;
	particle.texture = nu::Resources().Get<nu::Texture>("textures/particle.png", nu::Engine::Get().GetRenderer());
	particle.lifespan = nu::RandomFloat(0.5f, 0.75f);
	particle.velocity = { nu::RandomFloat(-100.0f, 100.0f), nu::RandomFloat(-100.0f, 100.0f) };

	nu::Engine::Get().GetPS().AddParticle(particle);
}
