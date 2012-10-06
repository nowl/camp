#include "game.hpp"

std::unique_ptr<Game> Game::_instance = nullptr;

Game *
Game::Instance()
{
    if(!_instance)
        _instance = std::unique_ptr<Game>(new Game());
    
    return _instance.get();
}

