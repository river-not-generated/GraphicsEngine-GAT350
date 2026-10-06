#pragma once

#include <memory>

namespace nu
{
	class Scene;

	class Game {
		public:
			Game() = default;
			virtual ~Game() = default;
			
			virtual bool Initialize() { return true; }
			virtual void Shutdown() {}
			virtual void Update(float dt);
			virtual void Draw(class Renderer& renderer);

			virtual void SetScene(std::unique_ptr<Scene> scene) { m_scene = std::move(scene); }

		protected:
			std::unique_ptr<Scene> m_scene;
			
	};
}