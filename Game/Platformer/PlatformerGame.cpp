#include "PlatformerGame.h"
#include "Engine.h"
#include "PlayerController.h"
#include "EnemyController.h"
#include "GoalController.h"
#include "Components/TilemapRendererComponent.h"
#include "Math/Vector2.h"

#include <memory>

using namespace nu;

bool PlatformerGame::Initialize() {
	nu::SetWorkingDirectory("Assets/PlatformerGame");
	Game::Initialize();

	m_scene = std::make_unique<Scene>();
	m_scene->SetGame(this);
	m_scene->Load("scenes/scene.json");

	// i'm tired of typing out the whole thing
	auto renderer = Engine::Get().GetRenderer();

	// AUDIO
	Engine::Get().GetAudio().AddSound("hurt", "audio/hurt.wav");
	Engine::Get().GetAudio().AddSound("attack", "audio/sword.wav");
	Engine::Get().GetAudio().AddSound("level", "audio/levelup.wav");
	Engine::Get().GetAudio().AddSound("enemyatk", "audio/enemyatk.wav");
	Engine::Get().GetAudio().AddSound("explode", "audio/explode.wav");
	Engine::Get().GetAudio().AddSound("select", "audio/select.wav");
	Engine::Get().GetAudio().AddSound("jump", "audio/jump.wav");
	Engine::Get().GetAudio().AddSound("fall", "audio/fall.wav");
	Engine::Get().GetAudio().AddSound("bell", "audio/cowbell.wav");


	// FONTS
	m_fonts["big"] = Resources().GetWithID<Font>("title-font", "fonts/bytesized.ttf", 96.0f);
	m_fonts["regular"] = Resources().GetWithID<Font>("reg-font", "fonts/tiny5.ttf", 54.0f);

	// TEXT
	m_text["title"] = new Text(m_fonts["big"]);
	m_text["title"]->Create(renderer, "A Generic Platformer Game", Colour{ 0,0,0 });

	m_text["move"] = new Text(m_fonts["regular"]);
	m_text["move"]->Create(renderer, "W and S to move", Colour{ 0,0,0 });
	m_text["jump"] = new Text(m_fonts["regular"]);
	m_text["jump"]->Create(renderer, "Space to jump", Colour{ 0,0,0 });
	m_text["attack"] = new Text(m_fonts["regular"]);
	m_text["attack"]->Create(renderer, "LMB to attack", Colour{ 0,0,0 });

	m_text["gameover"] = new Text(m_fonts["big"]);
	m_text["gameover"]->Create(renderer, "Game Over", Colour{ 0,0,0 });


	Engine::Get().GetAudio().PlaySound("bell");

	return true;
}

void PlatformerGame::Update(float dt) {
	switch (m_gamestate) {
		case PlatformerGame::GameState::Title:
			if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE)) {
				m_gamestate = GameState::StartGame; 
				Engine::Get().GetAudio().PlaySound("select");
			}
			break;
		case PlatformerGame::GameState::StartGame:
			ResetGame();
			m_gamestate = GameState::BuildLevel;
			break;
		case PlatformerGame::GameState::BuildLevel:
			CreateLevel(m_level);
			m_gamestate = GameState::StarLevel;
			break;
		case PlatformerGame::GameState::StarLevel:
			SpawnEntities(m_level);
			m_gamestate = GameState::Game;
			break;
		case PlatformerGame::GameState::Game:
			m_enemySpawnTimer -= dt;
			if (m_enemySpawnTimer <= 0.0f) {
				SpawnEnemy();
			}
			if (!m_scene->GetActorByTag("Player")) {
				SpawnPlayer(GetPlayerCoords(m_level).x, GetPlayerCoords(m_level).y, m_firstSpawn);
			}
			break;
		case PlatformerGame::GameState::EndLevel:
			m_scene->RemoveAllActors();
			m_gamestate = GameState::BuildLevel;
			break;
		case PlatformerGame::GameState::GameOver:
		{
			m_deathTimer -= (1.0 / 60.0f);
			if (m_deathTimer <= 0) {
				m_scene->RemoveAllActors(true);
				m_gamestate = GameState::WaitForRestart;
			}
		}
			break;
		case PlatformerGame::GameState::WaitForRestart:
			break;
		default:
			break;
	}

	Game::Update(dt);
}

void PlatformerGame::Draw(nu::Renderer& renderer) {
	// draw background first
	renderer.DrawTexture(*Resources().Get<Texture>("textures/background_layer_1.png", renderer), 0.0f, 0.0f, 0.0f, 6.0f);
	renderer.DrawTexture(*Resources().Get<Texture>("textures/background_layer_2.png", renderer), 0.0f, 0.0f, 0.0f, 6.0f);
	renderer.DrawTexture(*Resources().Get<Texture>("textures/background_layer_3.png", renderer), 0.0f, 0.0f, 0.0f, 6.0f);
	Game::Draw(renderer);

	switch (m_gamestate) {
		case PlatformerGame::GameState::Title:
			m_text["title"]->Draw(renderer, 350, 300);
			m_text["move"]->Draw(renderer, 750, 450);
			m_text["jump"]->Draw(renderer, 775, 525);
			m_text["attack"]->Draw(renderer, 775, 600);
			break;
		case PlatformerGame::GameState::StartGame:
		case PlatformerGame::GameState::BuildLevel:
		case PlatformerGame::GameState::StarLevel:
		case PlatformerGame::GameState::Game:
		case PlatformerGame::GameState::EndLevel:
			{
				auto actor = m_scene->GetActorByTag<PlayerController>("Player");
				if (actor) {
					for (int i = 0; i < actor->GetHealth(); i++) {
						renderer.DrawTexture(*Resources().Get<Texture>("textures/heart.png", renderer), renderer.GetWindowWidth() - 100.0f - (32.0f * i), 40.0f, 0.0f, 1.5f);
					}
				}
			}
			break;
		case PlatformerGame::GameState::GameOver:
			break;
		case PlatformerGame::GameState::WaitForRestart:
			m_text["gameover"]->Draw(renderer, 775, 300);
			break;
		default:
			break;
	}
}

void PlatformerGame::SpawnPlayer(float x, float y, bool useDefaultHealth)
{
	auto actor = Factory::Instance().Create<PlayerController>("PlayerPrototype");
	if (!useDefaultHealth) {
		actor->SetHealth(m_playerHealth);

	}
	actor->SetPosition({ x, y });
	m_scene->AddActor(std::move(actor));
}

void PlatformerGame::SpawnGoal(float x, float y) {
	auto actor = Factory::Instance().Create<GoalController>("GoalPrototype");
	actor->SetPosition({ x, y });
	m_scene->AddActor(std::move(actor));
}

void PlatformerGame::SpawnEnemy() {
	auto actor = Factory::Instance().Create<EnemyController>("EyePrototype");
	actor->SetPosition({ Engine::Get().GetRenderer().GetWindowWidth() / 2.0f, 0.0f});
	m_scene->AddActor(std::move(actor));
	m_enemySpawnTimer = 5.0f;
}

void PlatformerGame::EndLevel() {
	m_gamestate = GameState::EndLevel;
	m_level++;
}

void PlatformerGame::ResetGame()
{
	m_level = 1;
	m_firstSpawn = true;
}

void PlatformerGame::OnPlayerDead() {
	m_gamestate = GameState::GameOver;
	SetDeathTimer(5.0f);
}

void PlatformerGame::CreateLevel(int level) {
	auto actor = Factory::Instance().Create<Actor>("Level" + std::to_string(level));
	m_scene->AddActor(std::move(actor));
	m_enemySpawnTimer = 3.0f;
}

nu::Vector2 PlatformerGame::GetPlayerCoords(int level) const {
	switch (level) {
	case 1:
		return { 400, 400 };
	case 2:
		return { 300, 600 };
	case 3:
		return { 1500, 150 };
	}
	return { 0, 0 };
}

void PlatformerGame::SpawnEntities(int level)
{
	// I tried making this compatible with the json
	// but it's 1:30am on the day this is due and I don't have the patience or the brain power left to keep debugging this one minor thing
	/*auto ts = m_scene->GetActorByName("Level" + std::to_string(level))->GetComponent<TilemapRendererComponent>();
	if (ts) {
		SpawnPlayer(ts->GetPlayerSpawn().x, ts->GetPlayerSpawn().y, level == 1);
		SpawnGoal(ts->GetGoalSpawn().x, ts->GetGoalSpawn().y);
	}*/
	Vector2 playerCoords = GetPlayerCoords(level);
	Vector2 goalCoords = { 0.0f, 0.0f };
	switch (level) {
	case 1:
		goalCoords = { 1750, 600 };
		break;
	case 2:
		goalCoords = { 200, 150 };
		break;
	case 3:
		goalCoords = { 10, 10 };
	}
	SpawnPlayer(playerCoords.x, playerCoords.y, m_firstSpawn);
	m_firstSpawn = false;
	SpawnGoal(goalCoords.x, goalCoords.y);

}
