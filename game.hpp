#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <memory>

#include "images.hpp"
#include "engine.hpp"
#include "image_loader.hpp"

class Game {
public:
    static Game* Instance();

    Engine engine;
    ImageNameCache imageNameCache;

    unsigned int cellWidth, cellHeight;
    std::string gameState;

private:
    Game();
    static std::unique_ptr<Game> _instance;
};

#endif  // __GAME_HPP__
