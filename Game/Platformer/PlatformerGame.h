#pragma once
#include "Framework/Game.h"
#include "Renderer/Text.h"
#include "Resources/ResourceManager.h"
#include <map>

namespace nu {
	struct Vector2;
}

class PlatformerGame : public nu::Game
{
public:
	enum class GameState {
		Title
		, StartGame
		, BuildLevel
		, StarLevel
		, Game
		, EndLevel
		, GameOver
		, WaitForRestart
	};

// methods that main() needs
public:
	PlatformerGame() = default;

	bool Initialize() override;
	void Update(float dt) override;
	void Draw(class nu::Renderer& renderer) override;

	void SetDeathTimer(float time) { m_deathTimer = time; }
	void SetRespawnTimer(float time) { m_respawnTimer = time; }

	int GetLevel() const { return m_level; }
	void SetHealth(int health) { m_playerHealth = health; }

	void OnPlayerDead();
	void EndLevel();
	nu::Vector2 GetPlayerCoords(int level) const;

// game-specific methods that shouldn't be accessed by main()
private:
	void CreateLevel(int level = 1);
	void SpawnEntities(int level);
	void SpawnPlayer(float x, float y, bool useDefaultHealth = false);
	void SpawnGoal(float x, float y);
	void SpawnEnemy();

	void ResetGame();

// game variables including all the fonts and text
// these should really be read in through json tbh
private:

	std::map<std::string, nu::res_t<nu::Font>> m_fonts;
	std::map<std::string, nu::Text*> m_text;

	GameState m_gamestate = GameState::Title;
	int m_playerHealth = 10;
	float m_respawnTimer = 0.0f;
	float m_deathTimer = 0.0f;
	float m_enemySpawnTimer = 0.0f;
	bool m_firstSpawn = true;

	int m_level = 1;
};