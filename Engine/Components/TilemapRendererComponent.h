#pragma once
#include "RendererComponent.h"
#include "Resources/ResourceManager.h"
#include "Math/Vector2.h"
#include "Renderer/Tilemap.h"
#include <string>

namespace nu
{
	class TilemapRendererComponent : public RendererComponent
	{
	public:
		TilemapRendererComponent() = default;
		TilemapRendererComponent(const TilemapRendererComponent& other);

		CLASS_PROTOTYPE(TilemapRendererComponent)

		void OnStart() override;
		void Draw(const Renderer& renderer) override;

		void Read(const json::value_t& value) override;

		const Vector2& GetPlayerSpawn() const { return m_playerSpawn; }
		const Vector2& GetGoalSpawn() const { return m_goalSpawn; }

	private:
		std::string m_tilemapName;
		res_t<class Tilemap> m_tilemap;

		std::vector<std::unique_ptr<class PhysicsBody>> m_physicsBodies;
		Vector2 m_playerSpawn = { 0.0f, 0.0f };
		Vector2 m_goalSpawn = { 0.0f, 0.0f };
	};
}
