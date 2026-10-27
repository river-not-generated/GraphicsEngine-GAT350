#pragma once

#include "Math/Random.h"
#include "Math/Vector2.h"
#include "Math/MathUtils.h"
#include "Core/StringUtils.h"
#include "Math/Transform.h"

#include "Renderer/Text.h"
#include "Renderer/Texture.h"

#include "Core/Factory.h"
#include "Resources/ResourceManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Pipeline.h"
#include "Renderer/VertexBuffer.h"
#include "Input/Input.h"
#include "Core/GameTime.h"
#include "Framework/Game.h"
#include "Serialization/Json.h"

#include "Framework/Actor.h"
#include "Renderer/Mesh.h"
#include "Framework/Scene.h"
#include "Audio/Audio.h"
#include "Core/File.h"

namespace nu
{
	class Engine {
		public:
			static Engine& Get() { static Engine engine; return engine; }

			bool Initialize(float width, float height);
			void Shutdown();

			void Update();

			Input& GetInput() { return m_input; }
			Renderer& GetRenderer() { return m_renderer; }
			Time& GetTime() { return m_time; }
			Audio& GetAudio() { return m_audio; }

			Engine(const Engine&) = delete;
			Engine& operator = (const Engine&) = delete;

		private:
			Engine() = default;

		private:
			Input m_input;
			Renderer m_renderer;
			Time m_time;
			Audio m_audio;
	};
}

void fnEngine();