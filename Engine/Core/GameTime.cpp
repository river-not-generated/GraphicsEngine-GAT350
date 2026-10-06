#include "pch.h"
#include "GameTime.h"

namespace nu
{
	Time::Time() {
		Reset();
	}

	void Time::Reset() {
		m_startTicks = SDL_GetTicksNS();
		m_frameTicks = SDL_GetTicksNS();
		m_currentTicks = SDL_GetTicksNS();
	}

	void Time::Tick() {
		m_currentTicks = SDL_GetTicksNS();
		m_deltaTicks = m_currentTicks - m_frameTicks; // delta = current - previous
		m_frameTicks = m_currentTicks; // set prev ticks
	}
}