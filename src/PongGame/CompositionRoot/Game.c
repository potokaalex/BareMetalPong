#include "GameFactory.h"

int _isExit; 

void Game_Start()
{
    GameFactory_Create();
}

void Game_Restart()
{
    GameFactory_Dispose();
    Game_Start();
}

void Game_Exit()
{
    GameFactory_Dispose();
    _isExit = 1;
}

void Game_Startup()
{
    GameFactory_CreateStaticData();
    GameFactory_CreateProgressData();
    Game_Start();
    
    while(!_isExit) {}
}
