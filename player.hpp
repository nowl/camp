#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "game_components.hpp"

class Player {
public:
    Renderable renderable;
    bool canMove;
    BoxRenderable boxRenderable;
    BoxRenderable selectedCellRenderable;
};

#endif  // __PLAYER_HPP__
