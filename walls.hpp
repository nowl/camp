#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include <vector>
#include <memory>

#include "game_components.hpp"

class RenderComponent;

class Walls {
public:
    std::vector<Renderable> renderEntities;
    std::vector< std::unique_ptr<RenderComponent> > renderComponents;
};

#endif  // __WALLS_HPP__
