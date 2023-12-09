#include "Game.h"
#include "Settings.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Game game(settings::screenWidth, settings::screenHeight, settings::fps, settings::title);

    // Main game loop
    while (!game.ShouldClose())
    {
        game.Tick();
    }

    return 0;
}