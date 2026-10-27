#include "pch.h"
#include "Engine.h"
#include "framework.h"

void fnEngine()
{
	std::cout << "engine connected" << std::endl;
}

namespace nu
{
	bool Engine::Initialize(float width, float height) {
		m_renderer.Initialize("Game Engine", width, height);
		m_input.Initialize();
		m_audio.Initialize();
		fnEngine();

		return true;
	}

	void Engine::Shutdown() {
		m_input.Shutdown();
		m_renderer.Shutdown();
	}

	void Engine::Update() {
		m_time.Tick();
		m_input.Update();
		m_audio.Update();
	}
}