#include "Game.h"
#include "Settings.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

Game* game = nullptr;

void UpdateDrawFrame(void)
{
    game->Tick();
}


int main(void)
{
    game = new Game(settings::screenWidth, settings::screenHeight, settings::fps, settings::title);

#ifdef PLATFORM_WEB
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!game->ShouldClose())
    {
        game->Tick();
    }
#endif

    delete game;
    return 0;
}