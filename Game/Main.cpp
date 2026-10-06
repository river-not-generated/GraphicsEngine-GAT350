#include "Engine.h"

#include "SpaceGame/SpaceGame.h"
#include "Platformer/PlatformerGame.h"

#include <iostream>
#include <vector>
#include <random>

using namespace nu;
using namespace std;

const float WIN_WIDTH = 1920.0f;
const float WIN_HEIGHT = 888.0f;

int main()
{   
    // initialize the engine
    if (Engine::Get().Initialize(WIN_WIDTH, WIN_HEIGHT) == false) return 0;

    unique_ptr<PlatformerGame> game = make_unique<PlatformerGame>();

    game->Initialize();

    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_ESCAPE) {
                quit = true;
            }
        }

        Engine::Get().Update();
        float dt = Engine::Get().GetTime().GetDeltaTime();

        game->Update(dt);

        Engine::Get().GetRenderer().SetColour(0, 0, 0);
        Engine::Get().GetRenderer().Clear(); // clear the renderer

        game->Draw(Engine::Get().GetRenderer());

        Engine::Get().GetPS().Draw(Engine::Get().GetRenderer());

        Engine::Get().GetRenderer().Present(); // render the screen
    }

    game.reset();
    // shut down the program cleanly upon exiting
    Engine::Get().Shutdown();

    return 0;

}
