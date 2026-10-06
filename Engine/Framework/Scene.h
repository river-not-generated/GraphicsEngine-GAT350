#pragma once
#include "Actor.h"

#include <vector>
#include <string>
#include <memory>

namespace nu
{
	class Actor;
	class Game;

	class Scene {
	public:
		void AddActor(std::unique_ptr<Actor> actor);
		void RemoveAllActors(bool forceAll = false);

		bool Load(const std::string& sceneName);

		void Update(float dt);

		void Draw(const class Renderer& renderer);

		template<typename T = Actor>
		T* GetActorByName(const std::string& name);

		template<typename T = Actor>
		T* GetActorByTag(const std::string& tag);

		void SetGame(Game* game) { m_game = game; }
		Game* GetGame() { return m_game; }

	private:
		void UpdateCollisions();

	private:
		std::vector<std::unique_ptr<Actor>> m_actors;
		std::vector<std::unique_ptr<Actor>> m_pendingActors;

		Game* m_game;
	};

	template<typename T>
	inline T* Scene::GetActorByName(const std::string& name) {
		for (auto& actor : m_actors) {
			T* actorT = dynamic_cast<T*>(actor.get());
			if (actorT && actorT->m_name == name) return actorT;
		}
		return nullptr;
	}

	template<typename T>
	inline T* Scene::GetActorByTag(const std::string& tag) {
		for (auto& actor : m_actors) {
			T* actorT = dynamic_cast<T*>(actor.get());
			if (actorT && actorT->m_tag == tag) return actorT;
		}
		return nullptr;
	}
}