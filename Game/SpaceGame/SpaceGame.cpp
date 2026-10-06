#include "SpaceGame.h"
#include "Engine.h"
#include "Player.h"
#include "Enemy.h"
#include "Powerup.h"
#include "Assets.h"

#include <memory>

using namespace nu;

bool SpaceGame::Initialize() {
    nu::SetWorkingDirectory("assets/SpaceGame");
    Game::Initialize();

    m_scene = std::make_unique<Scene>();
    m_scene->SetGame(this);
    m_scene->Load("data/scene.json");

    // i'm tired of typing out the whole thing
    auto renderer = Engine::Get().GetRenderer();

    // AUDIO
    Engine::Get().GetAudio().AddSound("bell", "audio/cowbell.wav");
    Engine::Get().GetAudio().AddSound("hurt", "audio/sndHurt.mp3");
    Engine::Get().GetAudio().AddSound("death", "audio/sndDeath.mp3");
    Engine::Get().GetAudio().AddSound("fire", "audio/sndFire.mp3");
    Engine::Get().GetAudio().AddSound("powerup", "audio/sndPowerupPickup.mp3");
    Engine::Get().GetAudio().AddSound("kill", "audio/sndAsteroidHit.mp3");
    Engine::Get().GetAudio().AddSound("blip", "audio/sndBlip.mp3");
    Engine::Get().GetAudio().AddSound("thrust", "audio/sndThrust.mp3");

    // FONTS
    m_fonts["big"] = Resources().GetWithID<Font>("title-font", "fonts/bytesized.ttf", 64.0f);
    m_fonts["regular"] = Resources().GetWithID<Font>("regular-font", "fonts/tiny5.ttf", 32.0f);
    m_fonts["medium"] = Resources().GetWithID<Font>("medium-font", "fonts/tiny5.ttf", 48.0f);

    // because of the amount of times I reuse fonts I didn't want to have to call the full resource manager thing every time
    // as that's just asking for inconsistencies
    // therefore maps
    // TITLE SCREEN TEXT
    m_text["title"] = new Text(m_fonts["big"]);
    m_text["title"]->Create(renderer, "Another Space Game", Colour{255,255,255});
    m_text["subtitle"] = new Text(m_fonts["regular"]);
    m_text["subtitle"]->Create(renderer, "The objectively worse sequel to the gamemaker one", Colour{ 255,255,255 });
    m_text["start"] = new Text(m_fonts["medium"]);
    m_text["start"]->Create(renderer, "Press enter to continue", Colour{ 255,255,255 });

    // GAME UI
    m_text["score"] = new Text(m_fonts["regular"]);
    m_text["score"]->Create(renderer, "Score: " + std::to_string(m_score), Colour{ 255,255,255 });
    m_text["fire"] = new Text(m_fonts["regular"]);
    m_text["fire"]->Create(renderer, "Space to fire", Colour{ 255,255,255 });
    m_text["dash"] = new Text(m_fonts["regular"]);
    m_text["dash"]->Create(renderer, "Left shift to dash", Colour{ 255,255,255 });


    // GAME OVER SCREEN TEXT
    m_text["gameover"] = new Text(m_fonts["big"]);
    m_text["gameover"]->Create(renderer, "Game Over", Colour{ 255,255,255 });
    m_text["finalscore"] = new Text(m_fonts["regular"]);
    m_text["finalscore"]->Create(renderer, "Final Score: " + std::to_string(m_score), Colour{ 255,255,255 });

    Engine::Get().GetAudio().PlaySound("bell");

    return true;
}

void SpaceGame::Update(float dt) {
    switch (m_gamestate) {
        case SpaceGame::GameState::Title:
            if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_RETURN)) {
                m_gamestate = GameState::StartGame;
            }
            break;
        case SpaceGame::GameState::StartGame:
            m_score = 0;
            m_lives = 3;
            m_spawnMod = 1.0f;
            m_killCount = 0;
            m_gamestate = GameState::StartLevel;
            m_stateTimer = 0.0f;
            break;
        case SpaceGame::GameState::StartLevel:
            m_stateTimer -= dt;
            if (m_stateTimer <= 0) {
                m_scene->RemoveAllActors();
                m_spawnTime = 1.0f;
                m_powerupSpawnTime = 5.0f;
                SpawnPlayer();
                Engine::Get().GetAudio().PlaySound("blip");
                m_gamestate = GameState::Game;
            }
            break;
        case SpaceGame::GameState::Game:
            m_spawnTime -= dt;
            if (m_spawnTime <= 0) {
                m_spawnTime = nu::RandomFloat(3.0f, 4.0f) * m_spawnMod;
                SpawnEnemy();
            }
            m_powerupSpawnTime -= dt;
            if (m_powerupSpawnTime <= 0) {
                m_powerupSpawnTime = nu::RandomFloat(12.5f, 15.0f);
                SpawnPowerup();
            }
            break;
        case SpaceGame::GameState::EndLevel:
            break;
        case SpaceGame::GameState::GameOver:
            if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_RETURN)) {
                m_scene->RemoveAllActors();
                m_gamestate = GameState::Title;
            }
            break;
        default:
            break;
    }

    Game::Update(dt);
}

void SpaceGame::Draw(nu::Renderer& renderer) {
    renderer.DrawTexture(*Resources().Get<Texture>("textures/background.png", renderer), 30.0f, 30.0f);
    Game::Draw(renderer);
    switch (m_gamestate) {
        case SpaceGame::GameState::Title:
            m_text["title"]->Draw(renderer, 225, 400);
            m_text["subtitle"]->Draw(renderer, 150, 500);
            m_text["start"]->Draw(renderer, 275, 600);
            break;
        case SpaceGame::GameState::StartGame:
        case SpaceGame::GameState::StartLevel:
        case SpaceGame::GameState::Game:
            m_text["score"]->Create(renderer, "Score: " + std::to_string(m_score), Colour{ 255,255,255 });
            m_text["score"]->Draw(renderer, 50, 50);
            for (int i = 0; i < m_lives; i++) {
                renderer.DrawTexture(*Resources().Get<Texture>("textures/life.png", renderer), renderer.GetWindowWidth() - 50.0f - (30.0f * i), 40.0f, 0.0f, 1.25f);
            }
            m_text["fire"]->Draw(renderer, 50, renderer.GetWindowHeight() - 100);
            m_text["dash"]->Draw(renderer, 50, renderer.GetWindowHeight() - 50);
            break;
        case SpaceGame::GameState::EndLevel:
            break;
        case SpaceGame::GameState::GameOver:
            m_text["gameover"]->Draw(renderer, 375, 400);
            m_text["finalscore"]->Create(renderer, "High Score: " + std::to_string(m_highScore), Colour{ 255,255,255 });
            m_text["finalscore"]->Draw(renderer, 400, 500);
            m_text["start"]->Draw(renderer, 275, 600);
            
            break;
        default:
            break;
    }
}

void SpaceGame::SpawnPlayer() {
    auto actor = Factory::Instance().Create<Actor>("PlayerPrototype");
    actor->SetPosition(Vector2{ Engine::Get().GetRenderer().GetWindowWidth() / 2, Engine::Get().GetRenderer().GetWindowHeight() / 2 });
    m_scene->AddActor(std::move(actor));
}

void SpaceGame::SpawnEnemy() {
    float winWidth = Engine::Get().GetRenderer().GetWindowWidth();
    float winHeight = Engine::Get().GetRenderer().GetWindowHeight();

    auto actor = Factory::Instance().Create<Actor>("EnemyPrototype");
    actor->SetPosition(
        Vector2{ RandomInt(0, 1) == 0 ? RandomFloat(0, winWidth / 4.0f) : RandomFloat(winWidth * (0.5f), winWidth)
        , RandomInt(0, 1) == 0 ? RandomFloat(0, winHeight / 4.0f) : RandomFloat(winHeight * (0.75f), winHeight), }
    );
    m_scene->AddActor(std::move(actor));
}

void SpaceGame::SpawnPowerup() {
    float winWidth = Engine::Get().GetRenderer().GetWindowWidth();
    float winHeight = Engine::Get().GetRenderer().GetWindowHeight();

    int num = RandomInt(0, 1);
    if (num == 0) {
        auto actor = Factory::Instance().Create<Actor>("TripleshotPrototype");
        actor->SetPosition(Vector2{ RandomFloat(0, winWidth), RandomFloat(0, winHeight) });
        m_scene->AddActor(std::move(actor));
    }
    else {
        auto actor = Factory::Instance().Create<Actor>("HeartPrototype");
        actor->SetPosition(Vector2{ RandomFloat(0, winWidth), RandomFloat(0, winHeight) });
        m_scene->AddActor(std::move(actor));
    }
}

void SpaceGame::OnPlayerDead() {
    m_lives -= 1;
    if (m_lives == 0) {
        m_gamestate = GameState::GameOver;
        Engine::Get().GetAudio().PlaySound("death");
        m_highScore = std::max(m_highScore, m_score);
    }
    else {
        m_gamestate = GameState::StartLevel;
        Engine::Get().GetAudio().PlaySound("hurt");
    }

    m_stateTimer = 3.0f;
}
