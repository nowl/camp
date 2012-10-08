#include "game.hpp"

std::unique_ptr<Game> Game::_instance = nullptr;

Game *
Game::Instance()
{
    if(!_instance)
        _instance = std::unique_ptr<Game>(new Game());
    
    return _instance.get();
}

Game::Game()
    : cellWidth(9*1.25), cellHeight(16*1.25),
      gameState("play")
{}
